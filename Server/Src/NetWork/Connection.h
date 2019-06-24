#pragma once

#include "Platform.h"
#include "../Common/MemoryStream.h"
#include "../Common/ObjectPool.h"

class EventPoller;
class NetPacket;
class Connection;

using ConnectionCloseHnadler = std::function<void(Connection*)>;
using ConnectionAttachHandler = std::function<void(Connection*)>;
using ConnectionMessageHandler = std::function<void(Connection*,NetPacket*)>;

class Connection
{
public:
	Connection();
	~Connection();
public:
	uint32_t getConnID() const;
	void attach(SOCKET fd, EventPoller* eventPoller);
	void resumeData();

	bool recvData();

	bool sendData(const void* data, size_t sz);

	void close();

	void setBindIndex(uint16_t bindIndex);

	//…Ë÷√ªÿµ˜Handler
	void setCloseHandler(ConnectionCloseHnadler handler);
	void setAttachHandler(ConnectionAttachHandler handler);
	void setMessageHandler(ConnectionMessageHandler handler);

private:
	bool onRecv(size_t bytes);
	bool onWrite(size_t bytes);

private:
	uint16_t bindIndex_;
	uint16_t roundValue_;
	SOCKET fd_;
	EventPoller*  eventPoller_;

	char recvBuffer[1024];

	char senddingBuffer[1024];
	CMemoryStream sendBuffer_;

	bool sendding_,readding;

	ConnectionCloseHnadler connectionCloseHnadler_;
	ConnectionAttachHandler connectionAttachHandler_;
	ConnectionMessageHandler connectionMessageHandler_;
};

