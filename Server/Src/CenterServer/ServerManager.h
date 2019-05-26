#pragma once

#include <boost/serialization/singleton.hpp>
#include <list>

struct SServerInfo
{
	enum EServerType
	{
		eServerTypeGate,
		eServerTypeLogin,
	};

	EServerType m_eServerType;
	std::string m_szServerAddress;
	int m_nPort;
	int m_nConnID; //连接ID
	uint32_t m_nServerID;	//中心服务器分配的服务器ID
};

class CServerManager : public boost::serialization::singleton<CServerManager>
{
public:
	CServerManager();
	~CServerManager();

	SServerInfo* activateServer(SServerInfo::EServerType eServerType, int nConnID,const std::string& szServerAddress = std::string(), int nPort = 0);

	const SServerInfo* findServerByConnID(int nConnID);
	const SServerInfo* findGateServerByServerID(uint32_t nServerID);

	std::list<const SServerInfo* > getAllGateServerInfo();
	std::list<const SServerInfo* > getAllLoginServerInfo();

	void serverDisconnect(uint32_t nServerID);
private:
	int getFreeServerID();
private:
	static uint32_t s_nServerID;
	std::list<SServerInfo*> m_listServerInfo;

};

