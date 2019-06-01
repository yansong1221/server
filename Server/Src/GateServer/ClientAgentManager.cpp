#include "ClientAgentManager.h"

#include "../CoreService/TCPManager.h"
#include "../MessageDefine/MessageCmd.h"
#include "LoginServerManager.h"

CClientAgent::CClientAgent(CTCPSeesion* pTCPSeesion)
	:m_pTCPSeesion(pTCPSeesion)
{

}

CClientAgent::~CClientAgent()
{

}

void CClientAgent::sendMessage(const std::string& buffer)
{
	if (m_pTCPSeesion->isConnectOk() == false)
	{
		assert(false);
		return;
	}
	m_pTCPSeesion->sendData(buffer.data(), buffer.length());
}

CClientAgentManager::CClientAgentManager(CTCPManager& TCPManager)
	:m_TCPManager(TCPManager)
{

}


CClientAgentManager::~CClientAgentManager()
{
}

void CClientAgentManager::onClientRelayToServerMessage(uint32_t nConnID, CNetPacket* pNetPacket)
{
	//登录服务器处理消息
	if ((uint16_t)CMD::EMainCmd::eMessageLoginServer == pNetPacket->getMsgHeader().unMainCmd)
	{
		CLoginServer* pLoginServer = CLoginServerManager::get_mutable_instance().findFreeLoginServer();
		if (pLoginServer == nullptr)
		{
			//暂时没有登录服务器
			return;
		}
		if (pLoginServer->relayMessage(nConnID, pNetPacket) == false)
		{
			//转发失败
			assert(false);
		}
	}
}

void CClientAgentManager::onServerRelayToClientMessage(uint32_t nConnID, CNetPacket* pNetPacket)
{
	CMD::GateServer::RelayHeader relayHeader;
	pNetPacket->getBody() >> relayHeader;

	auto pClientAgent =  findClientAgentByConnID(relayHeader.nClientConnID);

	if (pClientAgent == nullptr)
	{
		assert(false);
		return;
	}

	std::string buffer = pNetPacket->getBody().readAll();
	pClientAgent->sendMessage(buffer);
}

CClientAgent* CClientAgentManager::findClientAgentByConnID(uint32_t nConnID)
{
	auto iter = m_mapClientAgent.find(nConnID);

	if (iter == m_mapClientAgent.end())
	{
		assert(false);
		return nullptr;
	}
	return iter->second;
}

