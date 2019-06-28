#pragma once

#include "Platform.h"
#include "../Common/MemoryStream.h"
#include "../Common/ObjectPool.h"

class EventPoller;
class NetPacket;
class Connection;


class Connection
{
public:

	using CloseHnadler = std::function<void(Connection*)>;
	using AttachHandler = std::function<void(Connection*)>;
	using MessageHandler = std::function<void(Connection*, NetPacket*)>;

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
	void setCloseHandler(CloseHnadler handler);
	void setAttachHandler(AttachHandler handler);
	void setMessageHandler(MessageHandler handler);

private:
	bool onRecv(size_t bytes);
	bool onWrite(size_t bytes);

private:
	uint16_t bindIndex_;
	uint16_t roundValue_;
	SOCKET fd_;
	EventPoller*  eventPoller_;

	char recvBuffer_[1024];
	char senddingBuffer_[1024];

	CMemoryStream sendBuffer_;

	bool sendding_,readding_;

	CloseHnadler connectionCloseHnadler_;
	AttachHandler connectionAttachHandler_;
	MessageHandler connectionMessageHandler_;
};

