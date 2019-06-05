#pragma once

#include <boost/serialization/singleton.hpp>
#include <cstdint>
#include <list>
#include "../CoreService/TCPManager.h"

class CLoginServer
{
public:
	CLoginServer(int32_t nServerID, CTCPSeesion* pTCPSeesion);
	~CLoginServer();

public:
	bool relayClientMessage(uint32_t nClientConnID, CNetPacket* pNetPacket);
	uint32_t getConnID() const;
	int32_t getServerID() const;
private:
	CTCPSeesion* m_pTCPSeesion;
	int32_t m_nServerID;
};

class CLoginServerManager : public boost::serialization::singleton<CLoginServerManager>
{
public:
	CLoginServerManager();
	~CLoginServerManager();

public:
	CLoginServer* addLoginServer(int32_t nServerID, CTCPSeesion* pTCPSeesion);

	CLoginServer* findLoginServerByServerID(int32_t nServerID);
	CLoginServer* findLoginServerByConnID(uint32_t nConnID);
	CLoginServer* findLoginServer(CTCPSeesion* pTCPSeesion);

	CLoginServer* findFreeLoginServer();

	void serverDisconnect(int32_t nConnID);
private:
	std::list<CLoginServer*> m_listLoginServer;
};

