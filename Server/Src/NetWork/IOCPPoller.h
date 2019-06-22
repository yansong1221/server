#pragma once

#include "Platform.h"
#include "../Common/ObjectPool.h"

#if PLATFORM_TYPE == PLATFORM_WIN

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
	WSABUF				wsaBuf;				// 数据缓冲

	SOCKET				listenFd;
	SOCKET				fd;					// 此IO操作对应的socket	
	IO_OPERATION_TYPE	ioType;				// IO操作类型

	size_t				availableSize;		//发送或者接收的大小

	IOContext()
	{
		reset();
	}

	void reset()
	{
		ZeroMemory(&overLapped, sizeof(overLapped));
		fd = INVALID_SOCKET;
		listenFd = INVALID_SOCKET;
		ZeroMemory(&wsaBuf, sizeof(wsaBuf));
		ioType = NULL_POSTED;
		availableSize = 0;
	}
};

class IOCPPoller
{
public:
	IOCPPoller();
	~IOCPPoller();

	bool startRecv(SOCKET fd, void* buffer, size_t sz);
	bool startAccept(SOCKET listenFd);
	bool startSend(SOCKET fd, void* buffer, size_t sz);

	int update();

	virtual bool triggerError(SOCKET fd);
	virtual bool triggerRead(SOCKET fd,size_t bytes); 
	virtual bool triggerAccept(SOCKET fd);
	virtual bool triggerWrite(SOCKET fd, size_t bytes);
private:

	bool AssociateCompletionPort(HANDLE fileHandle);
private:
	HANDLE completionPort_;
	ObjectPool<IOContext> IOContextPool_;
	
};

#endif

