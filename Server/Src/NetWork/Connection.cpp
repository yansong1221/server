#include "Connection.h"
#include "EventPoller.h"
#include "CommonFunc.h"



Connection::Connection(uint16_t index)
	:index_(index),
	round_(0)
{
	resumeData();
}


Connection::~Connection()
{
}

uint32_t Connection::getConnID() const
{
	return MAKE_UINT32(index_, round_);
}

void Connection::attach(SOCKET fd, IEventPoller* eventPoller)
{
	fd_ = fd;
	eventPoller_ = eventPoller;
}


void Connection::resumeData()
{
	fd_ = INVALID_SOCKET;
	round_++;
	sendding_ = false;
	readding = false;
}

bool Connection::recvData()
{
	if (!eventPoller_->asyncRecv(fd_, recvBuffer, sizeof(recvBuffer),
		std::bind(&Connection::onRecv, this, std::placeholders::_1)))
	{
		return false;
	}
	readding = true;
	return true;
}

bool Connection::sendData(const void* data, size_t sz)
{
	if (data != nullptr || sz != 0)
	{
		sendBuffer_.appendBinary(data, sz);
	}
	
	if (sendding_ || sendBuffer_.empty())
	{
		return true;
	}

	size_t copySize = min(sendBuffer_.size(), sizeof(senddingBuffer));
	CopyMemory(&senddingBuffer, sendBuffer_.data(), copySize);

	if (!eventPoller_->asyncSend(fd_, senddingBuffer, copySize,
		std::bind(&Connection::onWrite, this, std::placeholders::_1)))
	{
		return false;
	}
	sendding_ = true;
	return true;
}

bool Connection::onRecv(size_t bytes)
{
	readding = false;

	//发生错误关闭连接
	if (bytes == 0)
	{
		return true;
	}

	if (recvData() == false)
	{
		return true;
	}
	return true;
}

bool Connection::onWrite(size_t bytes)
{
	sendding_ = false;
	if (bytes == 0)
	{
		return true;
	}
	sendBuffer_.remove(0, bytes);
	if (!sendData(nullptr, 0))
	{
		return true;
	}

	return true;
}

ConnectionManager::ConnectionManager(size_t maxConn)
{
	for (uint16_t index = 0; index < maxConn; ++index)
	{
		freeConnections_.push_back(new Connection(index));
	}
}

ConnectionManager::~ConnectionManager()
{

}

Connection* ConnectionManager::createConnection()
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
