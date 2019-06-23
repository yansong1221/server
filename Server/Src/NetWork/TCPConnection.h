#pragma once

#include "Platform.h"

class IEventPoller;

class TCPConnection
{
public:
	TCPConnection(uint16_t index);
	~TCPConnection();
public:
	uint32_t getConnID() const;
	void attach(SOCKET fd, IEventPoller* eventPoller);
	void resumeData();

	void recvData();

private:
	bool onRecv(size_t bytes);
private:
	uint16_t index_;
	uint16_t round_;
	SOCKET fd_;
	IEventPoller*  eventPoller_;

	char buffer[1024];
};

class TCPConnectionManager
{
public:
	TCPConnectionManager(size_t maxConn);
	~TCPConnectionManager();

public:
	TCPConnection* createConnection();

private:
	std::list<TCPConnection*> freeConnections_;
	std::list<TCPConnection*> activeConnections_;
};

