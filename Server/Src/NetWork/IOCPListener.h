#pragma once

#include "Platform.h"
#include "../Common/Thread.h"
#include "../Common/ObjectPool.h"
#include "IOCPPoller.h"

class IOCPListener : public Thread,public IOCPPoller
{
public:
	IOCPListener();
	virtual ~IOCPListener();
protected:
	virtual bool onThreadStart() override;
	virtual	void onThreadEnd() override;
	virtual bool onThreadRun() override;

protected:
	virtual bool triggerRead(SOCKET fd, size_t bytes);
	virtual bool triggerAccept(SOCKET fd);
	virtual bool triggerWrite(SOCKET fd, size_t bytes);

public:
	virtual void setListenAddress(const std::string& address, int port);	
private:
	SOCKET listenFd_;
	char buffer[336500];
};

