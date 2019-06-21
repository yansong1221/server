#pragma once

#include "Platform.h"
#include "../Common/Thread.h"
#include "../Common/ObjectPool.h"

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

	SOCKET				ioSocket;			// ��IO������Ӧ��socket	
	IO_OPERATION_TYPE	ioType;				// IO��������

	CHAR				dataBuffer[1024];	//��������buffer
	size_t				availableSize;		//���ͻ��߽��յĴ�С

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
		availableSize = 0;
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
	//Ͷ������
	bool postAccept(IOContext* ioContext);
	bool postRecv(IOContext* ioContext);
	bool postSend(IOContext* ioContext);

	bool doAccept(IOContext* ioContext);
	bool doRecv(IOContext* ioContext);
	bool doSend(IOContext* ioContext);
	
private:
	SOCKET listenFd_;
	HANDLE completionPort_;

	ObjectPool<IOContext> IOContextPool_;

	LPFN_ACCEPTEX			fnAcceptEx;				//AcceptEx����ָ��														
	LPFN_GETACCEPTEXSOCKADDRS	fnGetAcceptExSockAddrs; //GetAcceptExSockAddrs;����ָ��
	
};

