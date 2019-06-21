#pragma once

#include "Platform.h"
#include "../Common/ObjectPool.h"

#if PLATFORM_TYPE == PLATFORM_WIN

enum IO_OPERATION_TYPE
{
	NULL_POSTED,		// ���ڳ�ʼ����������
	ACCEPT_POSTED,		// Ͷ��Accept����
	SEND_POSTED,		// Ͷ��Send����
	RECV_POSTED,		// Ͷ��Recv����
};

class IOContext
{
public:
	WSAOVERLAPPED		overLapped;			// ÿ��socket��ÿһ��IO��������Ҫһ���ص��ṹ
	WSABUF				wsaBuf;				// ���ݻ���

	SOCKET				fd;					// ��IO������Ӧ��socket	
	IO_OPERATION_TYPE	ioType;				// IO��������

	size_t				availableSize;		//���ͻ��߽��յĴ�С

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

