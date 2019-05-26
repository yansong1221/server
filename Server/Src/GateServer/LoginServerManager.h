#pragma once

#include <boost/serialization/singleton.hpp>
#include <cstdint>
#include <list>

class CLoginServer
{
public:
	CLoginServer(int32_t nServerID, int nConnID);
	~CLoginServer();

public:
	int32_t m_nServerID;
	int m_nConnID;
};

class CLoginServerManager : public boost::serialization::singleton<CLoginServerManager>
{
public:
	CLoginServerManager();
	~CLoginServerManager();

public:
	CLoginServer* addLoginServerInfo(int32_t nServerID, int nConnID);

	CLoginServer* findLoginServerByServerID(int32_t nServerID);
	CLoginServer* findLoginServerByConnID(int nConnID);

	void serverDisconnect(int nConnID);
private:
	std::list<CLoginServer*> m_listLoginServer;
};

