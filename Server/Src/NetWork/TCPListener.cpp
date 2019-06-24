
#include "TCPListener.h"
#include "Platform.h"
#include "CommonFunc.h"


TCPListener::TCPListener()
	:fd_(CommonFunc::createSocket()),
	listenerCloseHandler_(nullptr),
	listenerMessageHandler_(nullptr),
	listenerNewConnectHandler_(nullptr),
	eventPoller_(nullptr)
{
	eventPoller_ = EventPoller::createEventPoller();
}

TCPListener::~TCPListener()
{
	CommonFunc::closeSocket(fd_);
	CommonFunc::clearSocket();
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

void TCPListener::setCloseHandler(TCPListenerCloseHandler handler)
{
	listenerCloseHandler_ = handler;
}

void TCPListener::setMessageHandler(TCPListenerMessageHandler handler)
{
	listenerMessageHandler_ = handler;
}

void TCPListener::setNewConnectHandler(TCPListenerNewConnectHandler handler)
{
	listenerNewConnectHandler_ = handler;
}

bool TCPListener::closeConnection(Connection* conn)
{
	if (!connections_.hasObject(conn))
	{
		assert(false);
		return false;
	}

	conn->close();
	return true;
}

Connection* TCPListener::findConnection(uint32_t connID)
{
	const auto& activeConns = connections_.getActiveObjects();

	auto iter = std::find_if(activeConns.begin(), activeConns.end(), [&](const Connection* conn)
	{
		return conn->getConnID() == connID;
	});

	if (iter == activeConns.end())
	{
		assert(false);
		return nullptr;
	}

	return *iter;
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
	std::this_thread::sleep_for(std::chrono::milliseconds(1));
	return true;
}

void TCPListener::onAccept(SOCKET fd)
{
	eventPoller_->asyncAccept(fd_, std::bind(&TCPListener::onAccept, this, std::placeholders::_1));

	auto conn = connections_.createObject();
	conn->setAttachHandler([this](Connection* conn)
	{
		if (listenerNewConnectHandler_)listenerNewConnectHandler_(conn->getConnID());
	});
	conn->setCloseHandler([this](Connection* conn)
	{
		if (listenerCloseHandler_)listenerCloseHandler_(conn->getConnID());
		connections_.reclaimObject(conn);
	});
	conn->setMessageHandler([this](Connection* conn, NetPacket* netPacket)
	{
		if (listenerMessageHandler_)listenerMessageHandler_(conn->getConnID(), netPacket);
	});

	conn->setBindIndex(roundValue_++);
	conn->attach(fd, eventPoller_);

	if (conn->recvData() == false)
	{
		conn->close();
	}
}
