#include "IOCPListener.h"
#include "CommonFunc.h"
#include <cassert>


IOCPListener::IOCPListener()
{
	listenFd_ = CommonFunc::createSocket();
	
}

IOCPListener::~IOCPListener()
{
}

bool IOCPListener::onThreadStart()
{
	return true;
}

void IOCPListener::onThreadEnd()
{
	
}

bool IOCPListener::onThreadRun()
{
	update();
	return true;
}

bool IOCPListener::triggerRead(SOCKET fd, size_t bytes)
{
	startSend(fd, buffer, sizeof(buffer));
	return true;
}

bool IOCPListener::triggerAccept(SOCKET fd)
{
	startRecv(fd,buffer, sizeof(buffer));
	return true;
}

bool IOCPListener::triggerWrite(SOCKET fd, size_t bytes)
{
	return true;
}

void IOCPListener::setListenAddress(const std::string& address, int port)
{
	sockaddr_in addr = { 0 };
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = ADDR_ANY;

	if (bind(listenFd_, (const struct sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR)
	{
		throw std::runtime_error("����󶨷�������");
	}

	if (::listen(listenFd_, 512) == SOCKET_ERROR)
	{
		throw std::runtime_error("�˿�������������ռ�ã�����ʧ��");
	}

	if (!startAccept(listenFd_))
	{
		return;
	}
}
