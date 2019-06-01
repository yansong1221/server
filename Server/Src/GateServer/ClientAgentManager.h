#pragma once



#include <cstdint>
#include <unordered_map>

class CTCPManager;
class CNetPacket;
class CTCPSeesion;

class CClientAgent
{
public:
	CClientAgent(CTCPSeesion* pTCPSeesion);
	~CClientAgent();

	void sendMessage(const std::string& buffer);
private:
	CTCPSeesion* m_pTCPSeesion;
};

class CClientAgentManager
{
public:
	CClientAgentManager(CTCPManager& TCPManager);
	~CClientAgentManager();
public:
	void onClientRelayToServerMessage(uint32_t nConnID, CNetPacket* pNetPacket);

	void onServerRelayToClientMessage(uint32_t nConnID, CNetPacket* pNetPacket);

	CClientAgent* findClientAgentByConnID(uint32_t nConnID);
private:
	CTCPManager& m_TCPManager;
	std::unordered_map<uint32_t, CClientAgent*> m_mapClientAgent;
};

