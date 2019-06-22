#pragma once

#include "Platform.h"

#if PLATFORM_TYPE == PLATFORM_WIN

#include "../Common/ObjectPool.h"
#include "EventPoller.h"

class IOCPPoller : public IEventPoller
{
public:
	IOCPPoller();
	~IOCPPoller();

	virtual bool asyncRecv(SOCKET fd, void* buffer, size_t sz, ReadHandler handler)override;
	virtual bool asyncAccept(SOCKET fd, AcceptHandler handler) override;
	virtual bool asyncSend(SOCKET fd, void* buffer, size_t sz, WriteHandler handler) override;

	virtual int update() override;

private:

	bool AssociateCompletionPort(HANDLE fileHandle);
private:
	HANDLE completionPort_;
	ObjectPool<IOContext> IOContextPool_;
	AcceptHandler acceptHandler_;
	ReadHandler readHandler_;
	WriteHandler writeHandler_;
};

#endif

