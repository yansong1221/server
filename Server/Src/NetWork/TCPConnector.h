#pragma once

#include "Platform.h"

class NetPacket;

class IEventConnectorDelegate
{
public:

	/**
	* ���µ�������Ϣ��
	*/
	virtual void onConnectorMessage(uint32_t connID, NetPacket* packet) = 0;
	/**
	* �����ӹر�
	*/
	virtual void onConnectorClose(uint32_t connID) = 0;
};

class TCPConnector
{
public:
	TCPConnector();
	~TCPConnector();
	
	uint32_t connetTo(const std::string& address, int port);

private:

};

