#include "IOCPPoller.h"
#include "CommonFunc.h"

#if  PLATFORM_TYPE == PLATFORM_WIN

IOCPPoller::IOCPPoller()
	:completionPort_(CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0)),
	acceptHandler_(nullptr)
{
	if (NULL == completionPort_)
	{
		throw std::runtime_error("CreateIoCompletionPort Failed");
	}
}


IOCPPoller::~IOCPPoller()
{
	if (completionPort_ != NULL && completionPort_ != INVALID_HANDLE_VALUE)
	{ 
		CloseHandle(completionPort_); 
		completionPort_ = INVALID_HANDLE_VALUE; 
	}
}

bool IOCPPoller::AssociateCompletionPort(HANDLE fileHandle)
{
	if (completionPort_ == NULL)
	{
		assert(false);
		return false;
	}

	PULONG_PTR completionKey = 0;
	if (NULL == CreateIoCompletionPort(fileHandle, completionPort_, (DWORD)completionKey, 0))
	{
		if (WSAGetLastError() != ERROR_INVALID_PARAMETER)
		{
			assert(false);
			return false;
		}
	}
	return true;
}

ReadHandler IOCPPoller::findReadHandler(SOCKET fd)
{
	auto iter = readHandler_.find(fd);
	if (iter == readHandler_.end()) return nullptr;
	return iter->second;
}

WriteHandler IOCPPoller::findWriteHandler(SOCKET fd)
{
	auto iter = writeHandler_.find(fd);
	if (iter == writeHandler_.end()) return nullptr;
	return iter->second;
}

bool IOCPPoller::triggerError(SOCKET fd)
{
	if (!triggerRead(fd, 0))
	{
		triggerWrite(fd, 0);
	}
	return true;
}

bool IOCPPoller::triggerRead(SOCKET fd, size_t bytes)
{
	auto handler = findReadHandler(fd);
	if (handler == nullptr) return false;
	handler(bytes);
	return true;
}

bool IOCPPoller::triggerWrite(SOCKET fd, size_t bytes)
{
	auto handler = findWriteHandler(fd);
	if (handler == nullptr) return false;
	handler(bytes);
	return true;
}

bool IOCPPoller::asyncRecv(SOCKET fd, void* buffer, size_t sz, ReadHandler handler)
{

	if (!AssociateCompletionPort((HANDLE)fd))
	{
		assert(false);
		return false;
	}

	DWORD dwFlags = 0, dwBytes = 0;

	IOContext* ioContext = IOContextPool_.createObject();
	ioContext->reset();
	ioContext->ioType = RECV_POSTED;
	ioContext->fd = fd;
	ioContext->wsaBuf.buf = (CHAR*)buffer;
	ioContext->wsaBuf.len = sz;

	readHandler_[fd] = handler;
	int nBytesRecv = WSARecv(ioContext->fd, &ioContext->wsaBuf, 1, &dwBytes, &dwFlags, &ioContext->overLapped, NULL);
	
	if ((SOCKET_ERROR == nBytesRecv) && (WSA_IO_PENDING != WSAGetLastError()))
	{
		IOContextPool_.reclaimObject(ioContext);
		return false;
	}

	
	return true;
}

bool IOCPPoller::asyncAccept(SOCKET fd, AcceptHandler handler)
{

	if (!AssociateCompletionPort((HANDLE)fd))
	{
		assert(false);
		return false;
	}
	
	GUID guidAcceptEx = WSAID_ACCEPTEX;
	LPFN_ACCEPTEX	fnAcceptEx;				//AcceptEx函数指针

	// 提取扩展函数指针
	DWORD dwBytes = 0;
	if (SOCKET_ERROR == WSAIoctl(
		fd,
		SIO_GET_EXTENSION_FUNCTION_POINTER,
		&guidAcceptEx,
		sizeof(guidAcceptEx),
		&fnAcceptEx,
		sizeof(fnAcceptEx),
		&dwBytes,
		NULL,
		NULL))
	{
		throw std::runtime_error("Get AcceptEx Func Address Failed");
	}

	//不使用buffer
	static CHAR buffer[8];

	IOContext* ioContext = IOContextPool_.createObject();

	ioContext->reset();
	ioContext->ioType = ACCEPT_POSTED;
	ioContext->fd = CommonFunc::createSocket();
	ioContext->wsaBuf.buf = buffer;

	if (INVALID_SOCKET == ioContext->fd)
	{
		IOContextPool_.reclaimObject(ioContext);
		return false;
	}

	acceptHandler_ = handler;

	// 将接收缓冲置为0,令AcceptEx直接返回,防止拒绝服务攻击
	if (false == fnAcceptEx(fd, ioContext->fd, ioContext->wsaBuf.buf, 0, sizeof(sockaddr_in) + 16, sizeof(sockaddr_in) + 16, &dwBytes, &ioContext->overLapped))
	{
		DWORD dwError = WSAGetLastError();
		if (WSA_IO_PENDING != dwError)
		{
			IOContextPool_.reclaimObject(ioContext);
			return false;
		}
	}	
	return true;
}

bool IOCPPoller::asyncSend(SOCKET fd, void* buffer, size_t sz, WriteHandler handler)
{
	if (!AssociateCompletionPort((HANDLE)fd))
	{
		assert(false);
		return false;
	}

	IOContext* ioContext = IOContextPool_.createObject();

	ioContext->reset();
	ioContext->fd = fd;
	ioContext->ioType = SEND_POSTED;
	ioContext->wsaBuf.buf = (CHAR*)buffer;
	ioContext->wsaBuf.len = sz;

	DWORD dwBytes = 0, dwFlags = 0;

	writeHandler_[fd] = handler;
	if (::WSASend(fd, &ioContext->wsaBuf, 1, &dwBytes, dwFlags, &ioContext->overLapped, NULL) != NO_ERROR)
	{
		if (WSAGetLastError() != WSA_IO_PENDING)
		{
			IOContextPool_.reclaimObject(ioContext);
			return false;
		}
	}
	return true;
}

int IOCPPoller::update()
{
	OVERLAPPED *overLapped = NULL;
	DWORD dwBytes = 0;
	IOContext *ioContext = NULL;
	PULONG_PTR completionKey = 0;

	for (;;)
	{
		BOOL bRet = GetQueuedCompletionStatus(completionPort_, &dwBytes, (PULONG_PTR)&completionKey, &overLapped, 0);
		// 读取传入的参数
		ioContext = CONTAINING_RECORD(overLapped, IOContext, overLapped);

		if (!bRet)
		{
			DWORD dwErr = GetLastError();
			// 如果是超时了，就再继续等吧  
			if (WAIT_TIMEOUT == dwErr)
			{
				return 0;
			}
			//主机异常退出
			else
			{
				triggerError(ioContext->fd);
			}
		}
		else
		{
			if (ioContext->ioType == ACCEPT_POSTED)
			{
				if (acceptHandler_) acceptHandler_(ioContext->fd);
			}
			else if (ioContext->ioType == RECV_POSTED)
			{
				triggerRead(ioContext->fd, dwBytes);
			}
			else if (ioContext->ioType == SEND_POSTED)
			{
				triggerWrite(ioContext->fd, dwBytes);
			}
			else
			{
				assert(false);
			}
		}

		IOContextPool_.reclaimObject(ioContext);
	}
	
}
#endif //  PLATFORM_TYPE == PLATFORM_WIN