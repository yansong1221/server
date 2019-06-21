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

	bool AssociateCompletionPort(HANDLE fileHandle);

	bool postRecv(SOCKET fd, void* buffer, size_t sz);

	int update();

	bool triggerError(SOCKET fd);
	bool triggerRead(SOCKET fd,size_t bytes);
private:
	bool doRecv(IOContext* ioContext);
private:
	HANDLE completionPort_;
	ObjectPool<IOContext> IOContextPool_;
};

#endif

