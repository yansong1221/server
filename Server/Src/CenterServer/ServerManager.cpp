#include "ServerManager.h"

#include "fmt/core.h"
#include "../CoreService/LogService.h"

CServerManager::CServerManager()
{
}


CServerManager::~CServerManager()
{
}
SServerInfo* CServerManager::activateServer(SServerInfo::EServerType eServerType, int nConnID, const std::string& szServerAddress /*= std::string()*/, int nPort /*= 0*/)
{
	try
	{
		if (findServerByConnID(nConnID) != nullptr)
		{
			CLogService::get_mutable_instance().logError(fmt::format("已经注册过的网关服务器[{}:{}],ConnID:{}", szServerAddress, nPort, nConnID));
			return false;
		}
		auto pServerInfo = new SServerInfo;
		pServerInfo->m_szServerAddress = szServerAddress;
		pServerInfo->m_nPort = nPort;
		pServerInfo->m_nConnID = nConnID;
		pServerInfo->m_nServerID = getFreeServerID();
		pServerInfo->m_eServerType = eServerType;

		m_listServerInfo.push_back(pServerInfo);

		return pServerInfo;
	}
	catch (const std::exception&  e)
	{
		CLogService::get_mutable_instance().logError(fmt::format("{}", e.what()));
		return nullptr;
	}

	return nullptr;
}

const SServerInfo* CServerManager::findServerByConnID(int nConnID)
{
	for (auto pGateServerInfo : m_listServerInfo)
	{
		if (pGateServerInfo->m_nConnID == nConnID)
		{
			return pGateServerInfo;
		}
	}
	return nullptr;
}

const SServerInfo* CServerManager::findGateServerByServerID(uint32_t nServerID)
{
	for (auto pGateServerInfo : m_listServerInfo)
	{
		if (pGateServerInfo->m_nServerID == nServerID)
		{
			return pGateServerInfo;
		}
	}
	return nullptr;
}

std::list<const SServerInfo* > CServerManager::getAllGateServerInfo()
{
	std::list<const SServerInfo*> listServerInfo;

	for (auto pServerInfo : m_listServerInfo)
	{
		if (pServerInfo->m_eServerType == SServerInfo::eServerTypeGate)
		{
			listServerInfo.push_back(pServerInfo);
		}
	}
	return listServerInfo;
}

std::list<const SServerInfo* > CServerManager::getAllLoginServerInfo()
{
	std::list<const SServerInfo*> listServerInfo;

	for (auto pServerInfo : m_listServerInfo)
	{
		if (pServerInfo->m_eServerType == SServerInfo::eServerTypeLogin)
		{
			listServerInfo.push_back(pServerInfo);
		}
	}
	return listServerInfo;
}

void CServerManager::serverDisconnect(uint32_t nServerID)
{
	for (auto iter = m_listServerInfo.begin(); iter != m_listServerInfo.end(); ++iter)
	{
		if ((*iter)->m_nServerID == nServerID)
		{
			delete (*iter);
			m_listServerInfo.erase(iter);
			return;
		}
	}
}

int CServerManager::getFreeServerID()
{
	int nFindCount = 0;

	do
	{
		if (nFindCount == 0xFFFF)
		{
			throw std::runtime_error("未找到合适的ServerID!");
		}

		s_nServerID = s_nServerID + 1 % 0xFFFF;
		if (s_nServerID == 0)
		{
			nFindCount++;
			continue;
		}

		bool bUsed = false;

		for (auto pGateServerInfo : m_listServerInfo)
		{
			if (pGateServerInfo->m_nServerID == s_nServerID)
			{
				bUsed = true;
				break;
			}
		}
		if (bUsed)
		{
			nFindCount++;
			continue;
		}
		return s_nServerID;

	} while (true);
}

uint32_t CServerManager::s_nServerID = 0;
