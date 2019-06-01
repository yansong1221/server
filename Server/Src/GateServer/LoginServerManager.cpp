#include "LoginServerManager.h"
#include "../MessageDefine/MessageCmd.h"

CLoginServer::CLoginServer(int32_t nServerID, CTCPSeesion* pTCPSeesion)
	:m_nServerID(nServerID),
	m_pTCPSeesion(pTCPSeesion)
{

}
CLoginServer::~CLoginServer()
{

}

bool CLoginServer::relayMessage(uint32_t nClientConnID, CNetPacket* pNetPacket)
{
	if (!m_pTCPSeesion->isConnectOk())
	{
		//连接已经销毁了
		assert(false);
		return false;
	}

	CNetPacket relayPacket;
	relayPacket.getMsgHeader().unMainCmd = (uint16_t)CMD::EMainCmd::eMessageGateServer;
	relayPacket.getMsgHeader().unSubCmd = (uint16_t)CMD::ESubGateCmd::eMessageRelay;

	CMD::GateServer::RelayHeader relayHeader;
	relayHeader.nClientConnID = nClientConnID;

	relayPacket.getBody() << relayHeader << pNetPacket->getMsgHeader();
	relayPacket.getBody().appendBinary(pNetPacket->getBody().data(), pNetPacket->getBody().size());
	

	std::string sendBuffer = relayPacket.getPackingData();
	m_pTCPSeesion->sendData(sendBuffer.data(), sendBuffer.length());
	return true;
}

uint32_t CLoginServer::getConnID() const
{
	return m_pTCPSeesion->getConnID();
}

int32_t CLoginServer::getServerID() const
{
	return m_nServerID;
}

CLoginServerManager::CLoginServerManager()
{
}


CLoginServerManager::~CLoginServerManager()
{
}

CLoginServer* CLoginServerManager::addLoginServer(int32_t nServerID, CTCPSeesion* pTCPSeesion)
{
	if (pTCPSeesion == nullptr)
	{
		assert(false);
		return nullptr;
	}

	if (findLoginServer(pTCPSeesion) || findLoginServerByConnID(nServerID))
	{
		assert(false);
		return nullptr;
	}

	auto pLoginServer = new CLoginServer(nServerID, pTCPSeesion);
	m_listLoginServer.push_back(pLoginServer);

	return pLoginServer;
}

CLoginServer* CLoginServerManager::findLoginServerByServerID(int32_t nServerID)
{
	for (auto pLoginServer : m_listLoginServer)
	{
		if (pLoginServer->getServerID() == nServerID)
		{
			return pLoginServer;
		}
	}

	return nullptr;
}

CLoginServer* CLoginServerManager::findLoginServerByConnID(uint32_t nConnID)
{
	for (auto pLoginServer : m_listLoginServer)
	{
		if (pLoginServer->getConnID() == nConnID)
		{
			return pLoginServer;
		}
	}
	return nullptr;
}

CLoginServer* CLoginServerManager::findLoginServer(CTCPSeesion* pTCPSeesion)
{
	for (auto pLoginServer : m_listLoginServer)
	{
		if (pLoginServer->getConnID() == pTCPSeesion->getConnID())
		{
			return pLoginServer;
		}
	}
	return nullptr;
}

CLoginServer* CLoginServerManager::findFreeLoginServer()
{
	if (m_listLoginServer.empty())
	{
		return nullptr;
	}
	return m_listLoginServer.front();;
}

void CLoginServerManager::serverDisconnect(int32_t nConnID)
{
	for (auto iter = m_listLoginServer.begin(); iter != m_listLoginServer.end(); ++iter)
	{
		if ((*iter)->getConnID() == nConnID)
		{
			delete (*iter);
			m_listLoginServer.erase(iter);
			return;
		}
	}
}

