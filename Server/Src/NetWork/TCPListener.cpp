
#include "TCPListener.h"
#include "Platform.h"
#include "CommonFunc.h"
#include "IOCPPoller.h"

TCPListener::TCPListener()
	:fd_(CommonFunc::createSocket()),
	eventListenerDelegate_(nullptr),
	eventPoller_(nullptr)
{
#if PLATFORM_TYPE == PLATFORM_WIN
	eventPoller_ = new IOCPPoller;
#endif
}

TCPListener::~TCPListener()
{
	closesocket(fd_);
	WSACleanup();
}

void TCPListener::listenAddress(const std::string& address, int port)
{
	sockaddr_in addr = { 0 };
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = ADDR_ANY;

	if (bind(fd_, (const struct sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR)
	{
		throw std::runtime_error("网络绑定发生错误");
	}

	if (::listen(fd_, 512) == SOCKET_ERROR)
	{
		throw std::runtime_error("端口正被其他服务占用，监听失败");
	}

	eventPoller_->asyncAccept(fd_,std::bind(&TCPListener::onAccept,this,std::placeholders::_1));
}

void TCPListener::setEventListenerDelegate(IEventListenerDelegate* eventListenerDelegate)
{
	eventListenerDelegate_ = eventListenerDelegate;
}

bool TCPListener::onThreadStart()
{
	return true;
}

void TCPListener::onThreadEnd()
{
	
}

bool TCPListener::onThreadRun()
{
	eventPoller_->update();
	return true;
}

void TCPListener::onAccept(SOCKET fd)
{
	eventPoller_->asyncAccept(fd_, std::bind(&TCPListener::onAccept, this, std::placeholders::_1));

	static char buff[1024] = { 0 };
	eventPoller_->asyncRecv(fd, buff, sizeof(buff), [this](SOCKET fd,size_t bytes)
	{
		std::cout << buff << std::endl;
		return true;
	});
}
