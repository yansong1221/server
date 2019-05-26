#include "LoginServerManager.h"


CLoginServer::CLoginServer(int32_t nServerID, int nConnID)
	:m_nServerID(nServerID),
	m_nConnID(nConnID)
{

}
CLoginServer::~CLoginServer()
{

}

CLoginServerManager::CLoginServerManager()
{
}


CLoginServerManager::~CLoginServerManager()
{
}

CLoginServer* CLoginServerManager::addLoginServerInfo(int32_t nServerID, int nConnID)
{
	if (findLoginServerByConnID(nConnID) || findLoginServerByConnID(nServerID))
	{
		assert(false);
		return nullptr;
	}

	auto pLoginServer = new CLoginServer(nServerID, nConnID);
	m_listLoginServer.push_back(pLoginServer);

	return pLoginServer;
}

CLoginServer* CLoginServerManager::findLoginServerByServerID(int32_t nServerID)
{
	for (auto pLoginServer : m_listLoginServer)
	{
		if (pLoginServer->m_nServerID == nServerID)
		{
			return pLoginServer;
		}
	}

	return nullptr;
}

CLoginServer* CLoginServerManager::findLoginServerByConnID(int nConnID)
{
	for (auto pLoginServer : m_listLoginServer)
	{
		if (pLoginServer->m_nConnID == nConnID)
		{
			return pLoginServer;
		}
	}
	return nullptr;
}

void CLoginServerManager::serverDisconnect(int nConnID)
{
	for (auto iter = m_listLoginServer.begin(); iter != m_listLoginServer.end(); ++iter)
	{
		if ((*iter)->m_nConnID == nConnID)
		{
			delete (*iter);
			m_listLoginServer.erase(iter);
			return;
		}
	}
}

