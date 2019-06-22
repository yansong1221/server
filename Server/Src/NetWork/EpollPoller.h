#pragma once

#include "Platform.h"

#if PLATFORM_TYPE == PLATFORM_LINUX

class EpollPoller
{
public:
	EpollPoller();
	~EpollPoller();

	bool startRecv(SOCKET fd, void* buffer, size_t sz);
	bool startAccept(SOCKET listenFd);
	bool startSend(SOCKET fd, void* buffer, size_t sz);

	int update();
private:
	bool doRegisterRead(SOCKET fd,bool cancel = false);
	bool doRegisterWrite(SOCKET fd, bool cancel = false);

	bool isRegistered(SOCKET fd);
private:
	int epfd_;
};

#endif

