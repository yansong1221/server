#pragma once

#include "Platform.h"

#if PLATFORM_TYPE == PLATFORM_WIN

#include "../Common/ObjectPool.h"
#include "EventPoller.h"

class IOCPPoller : public EventPoller
{
public:
	IOCPPoller();
	~IOCPPoller();

	virtual bool asyncRecv(SOCKET fd, void* buffer, size_t sz, ReadHandler handler)override;
	virtual bool asyncAccept(SOCKET fd, AcceptHandler handler) override;
	virtual bool asyncSend(SOCKET fd, void* buffer, size_t sz, WriteHandler handler) override;
	virtual bool asyncConnect(SOCKET fd, const std::string& address, int port, ConnectHandler handler)override;

	virtual int update() override;

private:

	bool AssociateCompletionPort(HANDLE fileHandle);

	ReadHandler findReadHandler(SOCKET fd);
	WriteHandler findWriteHandler(SOCKET fd);
	ConnectHandler findConnectHandler(SOCKET fd);
	AcceptHandler findAcceptHandler(SOCKET fd);

	bool triggerError(SOCKET fd);
	bool triggerRead(SOCKET fd,size_t bytes);
	bool triggerWrite(SOCKET fd, size_t bytes);
	bool triggerConnect(SOCKET fd, bool success);
	bool triggerAccept(SOCKET listenFd,SOCKET connFd);

private:
	HANDLE completionPort_;
	ObjectPool<IOContext> IOContextPool_;

	std::unordered_map<SOCKET, AcceptHandler> acceptHandler_;
	std::unordered_map<SOCKET, ReadHandler>  readHandler_;
	std::unordered_map <SOCKET,WriteHandler> writeHandler_;
	std::unordered_map<SOCKET, ConnectHandler> connectHandler_;
};

#endif

