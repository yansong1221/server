#include "EpollPoller.h"

#if PLATFORM_TYPE == PLATFORM_LINUX

EpollPoller::EpollPoller()
	:epfd_(epoll_create(0))
{
	if (epfd_ == -1)
	{
		throw std::runtime_error("epoll_create failed");
	}
}


EpollPoller::~EpollPoller()
{

}

int EpollPoller::update()
{
	const static int MAX_EVENTS = 10;
	struct epoll_event events[MAX_EVENTS];

	int nfds = epoll_wait(epfd_, events, MAX_EVENTS, 0);

	for (int i = 0; i < nfds; ++i)
	{
		if (events[i].events & (EPOLLERR | EPOLLHUP))
		{
			this->triggerError(events[i].data.fd);
		}
		else
		{
			if (events[i].events & EPOLLIN)
			{
				this->triggerRead(events[i].data.fd);
			}

			if (events[i].events & EPOLLOUT)
			{
				this->triggerWrite(events[i].data.fd);
			}
		}
	}

}

#endif