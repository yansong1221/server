#pragma once

#include "Platform.h"

#if PLATFORM_TYPE == PLATFORM_LINUX

class EpollPoller
{
public:
	EpollPoller();
	~EpollPoller();

	int update();

private:
	int epfd_;
};

#endif

