#pragma once

#include "Platform.h"
#include "../Common/MemoryStream.h"

class EventPoller;

class Connection
{
public:
	Connection(uint16_t index);
	~Connection();
public:
	uint32_t getConnID() const;
	void attach(SOCKET fd, EventPoller* eventPoller);
	void resumeData();

	bool recvData();

	bool sendData(const void* data, size_t sz);
private:
	bool onRecv(size_t bytes);
	bool onWrite(size_t bytes);
private:
	uint16_t index_;
	uint16_t round_;
	SOCKET fd_;
	EventPoller*  eventPoller_;

	char recvBuffer[1024];

	char senddingBuffer[1024];
	CMemoryStream sendBuffer_;

	bool sendding_,readding;
};

class ConnectionManager
{
public:
	ConnectionManager(size_t maxConn);
	~ConnectionManager();

public:
	Connection* createConnection();

private:
	std::list<Connection*> freeConnections_;
	std::list<Connection*> activeConnections_;
};

