#include "Connection.h"
#include "EventPoller.h"
#include "CommonFunc.h"
#include "TCPListener.h"
#include "NetPacket.h"
#include <functional>


Connection::Connection()
	:bindIndex_(0),
	roundValue_(0),
	connectionCloseHnadler_(nullptr),
	connectionAttachHandler_(nullptr),
	connectionMessageHandler_(nullptr)
{
	resumeData();
}


Connection::~Connection()
{
}

uint32_t Connection::getConnID() const
{
	return MAKE_UINT32(bindIndex_, roundValue_);
}

void Connection::attach(SOCKET fd, EventPoller* eventPoller)
{
	fd_ = fd;
	eventPoller_ = eventPoller;

	connectionAttachHandler_(this);
}


void Connection::resumeData()
{
	fd_ = INVALID_SOCKET;
	roundValue_++;
	sendding_ = false;
	readding = false;
	sendBuffer_.clear();
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
	if (data != nullptr && sz != 0)
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

void Connection::close()
{
	CommonFunc::closeSocket(fd_);

	if (!sendding_ && !readding)
	{
		if(connectionCloseHnadler_)connectionCloseHnadler_(this);
		resumeData();
	}
}

void Connection::setBindIndex(uint16_t bindIndex)
{
	bindIndex_ = bindIndex;
}

void Connection::setCloseHandler(ConnectionCloseHnadler handler)
{
	connectionCloseHnadler_ = handler;
}

void Connection::setAttachHandler(ConnectionAttachHandler handler)
{
	connectionAttachHandler_ = handler;
}

void Connection::setMessageHandler(ConnectionMessageHandler handler)
{
	connectionMessageHandler_ = handler;
}

bool Connection::onRecv(size_t bytes)
{
	readding = false;

	//发生错误关闭连接
	if (bytes == 0 || recvData() == false)
	{
		close();
		return true;
	}

	sendData(recvBuffer, bytes);
	close();
	return true;
}

bool Connection::onWrite(size_t bytes)
{
	sendding_ = false;
	sendBuffer_.remove(0, bytes);

	if (bytes == 0 || !sendData(nullptr, 0))
	{
		close();
		return true;
	}
	
	return true;
}



