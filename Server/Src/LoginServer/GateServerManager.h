#pragma once

#include <boost/serialization/singleton.hpp>
#include <string>
#include <list>

class CGateServer
{
public:
	enum EConnectStatus
	{
		eConnectStatusNull,
		eConnectStatusConnecting,
		eConnectStatusSuccess,
	};
public:
	CGateServer(const std::string& szAddress, int nPort, int32_t nServerID);
	~CGateServer();

	bool isConnectOk() const;
public:
	std::string m_szAddress;
	int m_nPort;
	int32_t m_nServerID;
	int m_nConnID;
	EConnectStatus m_eConnectStatus;

};
class CGateServerManager : public boost::serialization::singleton<CGateServerManager>
{
public:
	CGateServerManager();
	~CGateServerManager();

public:

	CGateServer* addGateServerInfo(const std::string& szAddress,int nPort,int32_t nServerID);

	CGateServer* findGateServerByServerID(int32_t nServerID);
	CGateServer* findGateServerByConnID(int nConnID);

	std::list<CGateServer*>& getAllGateServer();

	void serverDisconnect(int nConnID);

	bool hasGateServer(CGateServer* pGateServer);

	void clear();

private:
	std::list<CGateServer*> m_listGateServer;
};

