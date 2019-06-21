#include "IOCPPoller.h"
#include "CommonFunc.h"

#if  PLATFORM_TYPE == PLATFORM_WIN

IOCPPoller::IOCPPoller()
	:completionPort_(CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0))
{
	if (NULL == completionPort_)
	{
		throw std::runtime_error("CreateIoCompletionPort Failed");
	}
}


IOCPPoller::~IOCPPoller()
{
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

bool IOCPPoller::postRecv(SOCKET fd, void* buffer, size_t sz)
{
	IOContext* ioContext = IOContextPool_.createObject();

	DWORD dwFlags = 0, dwBytes = 0;
	ioContext->ioType = RECV_POSTED;
	ioContext->fd = fd;
	ioContext->wsaBuf.buf = (CHAR*)buffer;
	ioContext->wsaBuf.len = sz;

	int nBytesRecv = WSARecv(ioContext->fd, &ioContext->wsaBuf, 1, &dwBytes, &dwFlags, &ioContext->overLapped, NULL);
	
	if ((SOCKET_ERROR == nBytesRecv) && (WSA_IO_PENDING != WSAGetLastError()))
	{
		IOContextPool_.reclaimObject(ioContext);
		return false;
	}
	return true;
}

int IOCPPoller::update()
{
	OVERLAPPED *overLapped = NULL;
	DWORD dwBytes = 0;
	IOContext *ioContext = NULL;
	PULONG_PTR completionKey = 0;

	BOOL bRet = GetQueuedCompletionStatus(completionPort_, &dwBytes, (PULONG_PTR)&completionKey, &overLapped, 0);

	if (!bRet)
	{
		DWORD dwErr = GetLastError();
		// 如果是超时了，就再继续等吧  
		if (WAIT_TIMEOUT == dwErr)
		{
			return 0;
		}

		return -1;
	}

	// 读取传入的参数
	ioContext = CONTAINING_RECORD(overLapped, IOContext, overLapped);
	ioContext->availableSize = dwBytes;
	
	if (ioContext->ioType == ACCEPT_POSTED)
	{
		//doAccept(ioContext);
	}
	else if (ioContext->ioType == RECV_POSTED)
	{
		doRecv(ioContext);
	}
	else if (ioContext->ioType == SEND_POSTED)
	{
		//doSend(ioContext);
	}
	else
	{
		assert(false);
	}
	
	return true;
}

bool IOCPPoller::triggerError(SOCKET fd)
{
	
}

bool IOCPPoller::triggerRead(SOCKET fd,size_t bytes)
{

}

bool IOCPPoller::doRecv(SOCKET fd, size_t bytes)
{
	triggerRead(fd, bytes);
	
	if (!postRecv(ioContext->fd, TODO, TODO))
	{
		triggerError(ioContext->fd);
	}

	IOContextPool_.reclaimObject(ioContext);
	return true;
}

#endif //  PLATFORM_TYPE == PLATFORM_WIN