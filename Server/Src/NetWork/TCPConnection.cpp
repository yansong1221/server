#include "TCPConnection.h"
#include "EventPoller.h"
#include "CommonFunc.h"



TCPConnection::TCPConnection(uint16_t index)
	:index_(index),
	round_(0)
{
	resumeData();
}


TCPConnection::~TCPConnection()
{
}

uint32_t TCPConnection::getConnID() const
{
	return MAKE_UINT32(index_, round_);
}

void TCPConnection::attach(SOCKET fd, IEventPoller* eventPoller)
{
	fd_ = fd;
	eventPoller_ = eventPoller;
}


void TCPConnection::resumeData()
{
	fd_ = INVALID_SOCKET;
	round_++;
}

void TCPConnection::recvData()
{
	eventPoller_->asyncRecv(fd_, buffer, sizeof(buffer), std::bind(&TCPConnection::onRecv, this, std::placeholders::_1));
}

bool TCPConnection::onRecv(size_t bytes)
{
	//发生错误关闭连接
	if (bytes == 0)
	{
		return true;
	}
	//继续接收
	eventPoller_->asyncRecv(fd_, buffer, sizeof(buffer), std::bind(&TCPConnection::onRecv, this, std::placeholders::_1));
	//解析数据

	return true;
}

TCPConnectionManager::TCPConnectionManager(size_t maxConn)
{
	for (uint16_t index = 0; index < maxConn; ++index)
	{
		freeConnections_.push_back(new TCPConnection(index));
	}
}

TCPConnectionManager::~TCPConnectionManager()
{

}

TCPConnection* TCPConnectionManager::createConnection()
{
	if (freeConnections_.empty())
	{
		return nullptr;
	}

	auto conn = freeConnections_.front();
	freeConnections_.pop_front();

	activeConnections_.push_back(conn);
	return conn;
}
