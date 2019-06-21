#pragma once

#include "Platform.h"
#include "../Common/Thread.h"
#include "../Common/ObjectPool.h"

enum IO_OPERATION_TYPE
{
	NULL_POSTED,		// 用于初始化，无意义
	ACCEPT_POSTED,		// 投递Accept操作
	SEND_POSTED,		// 投递Send操作
	RECV_POSTED,		// 投递Recv操作
};

class IOContext
{
public:
	WSAOVERLAPPED		overLapped;			// 每个socket的每一个IO操作都需要一个重叠结构
	SOCKET				ioSocket;			// 此IO操作对应的socket
	WSABUF				wsaBuf;				// 数据缓冲
	IO_OPERATION_TYPE	ioType;				// IO操作类型
	CHAR				dataBuffer[1024];	//保存数据buffer

	IOContext()
	{
		reset();
	}

	void reset()
	{
		ZeroMemory(&overLapped, sizeof(overLapped));
		ioSocket = INVALID_SOCKET;
		ZeroMemory(&wsaBuf, sizeof(wsaBuf));
		ioType = NULL_POSTED;
		ZeroMemory(dataBuffer, sizeof(dataBuffer));

		wsaBuf.buf = dataBuffer;
		wsaBuf.len = sizeof(dataBuffer);
	}
};

class IOCPListener : public Thread
{
public:
	IOCPListener();
	virtual ~IOCPListener();
protected:
	virtual bool onThreadStart() override;
	virtual	void onThreadEnd() override;
	virtual bool onThreadRun() override;

public:
	virtual void setListenAddress(const std::string& address, int port);

private:
	void InitFunction();

	bool AssociateCompletionPort(HANDLE fileHandle);
	//投递任务
	bool postAccept(IOContext* ioContext);
	bool postRecv(IOContext* ioContext);

	bool doAccept(IOContext* ioContext);
	bool doRecv(IOContext* ioContext);
	bool postSend(IOContext* ioContext);
private:
	SOCKET listenFd_;
	HANDLE completionPort_;

	ObjectPool<IOContext> IOContextPool_;

	LPFN_ACCEPTEX			fnAcceptEx;				//AcceptEx函数指针														
	LPFN_GETACCEPTEXSOCKADDRS	fnGetAcceptExSockAddrs; //GetAcceptExSockAddrs;函数指针
	
};

