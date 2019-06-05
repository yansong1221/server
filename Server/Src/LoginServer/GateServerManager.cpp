#include "GateServerManager.h"


CGateServer::CGateServer(const std::string& szAddress, int nPort, int32_t nServerID)
	:m_nConnID(0),
	m_eConnectStatus(eConnectStatusNull),
	m_szAddress(szAddress),
	m_nPort(nPort),
	m_nServerID(nServerID)
{

}
CGateServer::~CGateServer()
{

}

bool CGateServer::isConnectOk() const
{
	return m_nConnID != 0;
}

CGateServerManager::CGateServerManager()
{
}


CGateServerManager::~CGateServerManager()
{
}

CGateServer* CGateServerManager::addGateServerInfo(const std::string& szAddress, int nPort, int32_t nServerID)
{
	auto pGateServer = new CGateServer(szAddress, nPort, nServerID);
	m_listGateServer.push_back(pGateServer);
	return pGateServer;
}

CGateServer* CGateServerManager::findGateServerByServerID(int32_t nServerID)
{
	for (auto pGateServer : m_listGateServer)
	{
		if (pGateServer->m_nServerID == nServerID)
		{
			return pGateServer;
		}
	}

	return nullptr;
}

CGateServer* CGateServerManager::findGateServerByConnID(uint32_t nConnID)
{
	for (auto pGateServer : m_listGateServer)
	{
		if (pGateServer->m_nConnID == nConnID)
		{
			return pGateServer;
		}
	}

	return nullptr;
}

std::list<CGateServer*>& CGateServerManager::getAllGateServer()
{
	return m_listGateServer;
}

void CGateServerManager::serverDisconnect(int nConnID)
{
	for (auto iter = m_listGateServer.begin(); iter != m_listGateServer.end(); ++iter)
	{
		if ((*iter)->m_nConnID == nConnID)
		{
			delete (*iter);
			m_listGateServer.erase(iter);
			return;
		}
	}
}

bool CGateServerManager::hasGateServer(CGateServer* pGateServer)
{
	auto iter = std::find(m_listGateServer.begin(), m_listGateServer.end(), pGateServer);

	if (iter == m_listGateServer.end()) return false;

	return true;
}

void CGateServerManager::clear()
{
	for (auto pGateServer : m_listGateServer)
	{
		delete pGateServer;
	}
	m_listGateServer.clear();
}
