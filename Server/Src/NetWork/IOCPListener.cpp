#include "IOCPListener.h"
#include "CommonFunc.h"
#include <cassert>


IOCPListener::IOCPListener()
{
	InitFunction();

	listenFd_ = CommonFunc::createSocket();
	completionPort_ = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);

	if (NULL == completionPort_)
	{
		throw std::runtime_error("CreateIoCompletionPort Failed");
	}

	// 将socket绑定到完成端口中
	if (AssociateCompletionPort((HANDLE)listenFd_))
	{
		throw std::runtime_error("Bind Listen Socket To IoCompletionPort Failed");	
	}	
}

IOCPListener::~IOCPListener()
{
}

bool IOCPListener::onThreadStart()
{
	return true;
}

void IOCPListener::onThreadEnd()
{

}

bool IOCPListener::onThreadRun()
{
	OVERLAPPED *overLapped = NULL;
	DWORD dwBytes = 0;
	IOContext *ioContext = NULL;
	PULONG_PTR completionKey = 0;

	BOOL bRet = GetQueuedCompletionStatus(completionPort_, &dwBytes, (PULONG_PTR)&completionKey, &overLapped, 0);

	// 读取传入的参数
	ioContext = CONTAINING_RECORD(overLapped, IOContext, overLapped);

	if (!bRet)
	{	
		return true;
	}
	
	// 判断是否有客户端断开
	if ((0 == dwBytes) && (RECV_POSTED == ioContext->ioType || SEND_POSTED == ioContext->ioType))
	{
	
		return true;
	}
	else
	{
		switch (ioContext->ioType)
		{
		case ACCEPT_POSTED:
			doAccept(ioContext);
			break;
		case RECV_POSTED:
			doRecv(ioContext);
			break;
		case SEND_POSTED:
			break;
		default:
			break;
		}
	}
	
	return true;
}

void IOCPListener::setListenAddress(const std::string& address, int port)
{
	sockaddr_in addr = { 0 };
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = ADDR_ANY;

	if (bind(listenFd_, (const struct sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR)
	{
		std::runtime_error("网络绑定发生错误");
	}

	if (::listen(listenFd_, 512) == SOCKET_ERROR)
	{
		std::runtime_error("端口正被其他服务占用，监听失败");
	}

	for (int i = 0; i < IOContextPool_.size(); ++i)
	{
		IOContext* ioContext = IOContextPool_.createObject();
		postAccept(ioContext);
	}
}

void IOCPListener::InitFunction()
{
	GUID guidAcceptEx = WSAID_ACCEPTEX;
	GUID guidGetAcceptSockAddrs = WSAID_GETACCEPTEXSOCKADDRS;
	// 提取扩展函数指针
	DWORD dwBytes = 0;
	if (SOCKET_ERROR == WSAIoctl(
		listenFd_,
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

	if (SOCKET_ERROR == WSAIoctl(
		listenFd_,
		SIO_GET_EXTENSION_FUNCTION_POINTER,
		&guidGetAcceptSockAddrs,
		sizeof(guidGetAcceptSockAddrs),
		&fnGetAcceptExSockAddrs,
		sizeof(fnGetAcceptExSockAddrs),
		&dwBytes,
		NULL,
		NULL))
	{
		throw std::runtime_error("Get GetAcceptSockAddrs Func Address Failed");
	}

}

bool IOCPListener::AssociateCompletionPort(HANDLE fileHandle)
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

bool IOCPListener::postAccept(IOContext* ioContext)
{
	DWORD dwBytes = 0;

	ioContext->reset();
	ioContext->ioType = ACCEPT_POSTED;
	ioContext->ioSocket = CommonFunc::createSocket();

	if (INVALID_SOCKET == ioContext->ioSocket)
	{
		return false;
	}

	// 将接收缓冲置为0,令AcceptEx直接返回,防止拒绝服务攻击
	if (false == fnAcceptEx(listenFd_, ioContext->ioSocket, ioContext->wsaBuf.buf, 0, sizeof(sockaddr_in) + 16, sizeof(sockaddr_in) + 16, &dwBytes, &ioContext->overLapped))
	{
		if (WSA_IO_PENDING != WSAGetLastError())
		{
			return false;
		}
	}
	return true;
}

bool IOCPListener::postRecv(IOContext* ioContext)
{
	DWORD dwFlags = 0, dwBytes = 0;
	ioContext->ioType = RECV_POSTED;

	int nBytesRecv = WSARecv(ioContext->ioSocket, &ioContext->wsaBuf, 1, &dwBytes, &dwFlags, &ioContext->overLapped, NULL);
	// 如果返回值错误，并且错误的代码并非是Pending的话，那就说明这个重叠请求失败了
	if ((SOCKET_ERROR == nBytesRecv) && (WSA_IO_PENDING != WSAGetLastError()))
	{
		return false;
	}
	return true;
}

bool IOCPListener::doAccept(IOContext* ioContext)
{
	SOCKADDR_IN *clientAddr = NULL;
	SOCKADDR_IN *localAddr = NULL;
	int clientAddrLen, localAddrLen;
	clientAddrLen = localAddrLen = sizeof(SOCKADDR_IN);

	fnGetAcceptExSockAddrs(ioContext->wsaBuf.buf, 0, localAddrLen, clientAddrLen, (LPSOCKADDR *)&localAddr, &localAddrLen, (LPSOCKADDR *)&clientAddr, &clientAddrLen);


	//投递接收数据
	IOContext* recvIOContext = IOContextPool_.createObject();
	recvIOContext->ioSocket = ioContext->ioSocket;

	//继续监听
	if (!postAccept(ioContext))
	{
		IOContextPool_.reclaimObject(ioContext);
	}

	if (!AssociateCompletionPort((HANDLE)recvIOContext->ioSocket))
	{
		IOContextPool_.reclaimObject(recvIOContext);
		return false;
	}
	if (!postRecv(recvIOContext))
	{
		IOContextPool_.reclaimObject(recvIOContext);
	}

	return true;
}

bool IOCPListener::doRecv(IOContext* ioContext)
{
	return true;
}

bool IOCPListener::postSend(IOContext* ioContext)
{
	ioContext->ioType = SEND_POSTED;
	DWORD dwBytes = 0, dwFlags = 0;

	if (::WSASend(ioContext->ioSocket, &ioContext->wsaBuf, 1, &dwBytes, dwFlags, &ioContext->overLapped, NULL) != NO_ERROR)
	{
		if (WSAGetLastError() != WSA_IO_PENDING)
		{
			return false;
		}
	}
}
