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

bool EpollPoller::startRecv(SOCKET fd, void* buffer, size_t sz)
{

}

bool EpollPoller::startAccept(SOCKET listenFd)
{

}

bool EpollPoller::startSend(SOCKET fd, void* buffer, size_t sz)
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

bool EpollPoller::doRegisterRead(SOCKET fd, bool cancel /*= false*/)
{
	struct epoll_event ev;
	memset(&ev, 0, sizeof(ev)); // stop valgrind warning
	int op;

	ev.data.fd = fd;

	if (!cancel)
	{
		op = isRegistered(fd) ? EPOLL_CTL_MOD : EPOLL_CTL_ADD;
	}
	else
	{
		op = EPOLL_CTL_DEL;
	}
	
	if (epoll_ctl(epfd_, op, fd, &ev) < 0)
	{
		return false;
	}

	return true;
}

bool EpollPoller::doRegisterWrite(SOCKET fd, bool cancel /*= false*/)
{
	struct epoll_event ev;
	memset(&ev, 0, sizeof(ev)); // stop valgrind warning
	int op;

	ev.data.fd = fd;

	if (!cancel)
	{
		op = isRegistered(fd) ? EPOLL_CTL_MOD : EPOLL_CTL_ADD;
		ev.events = EPOLLIN | EPOLLOUT;
	}
	else
	{
		op = EPOLL_CTL_MOD;
		ev.events = EPOLLIN;
	}

	if (epoll_ctl(epfd_, op, fd, &ev) < 0)
	{
		return false;
	}
}

bool EpollPoller::isRegistered(SOCKET fd)
{
	return true;
}

#endif