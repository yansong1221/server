#include "GateServerConfig.h"



CGateServerConfig::CGateServerConfig()
{
}


CGateServerConfig::~CGateServerConfig()
{
}

bool CGateServerConfig::loadConfig()
{
	m_serverConfigInfo.m_centerServerAddress.m_szServerAddress = "127.0.0.1";
	m_serverConfigInfo.m_centerServerAddress.m_nServerPort = 8800;

	m_serverConfigInfo.m_localAddress.m_szServerAddress = "127.0.0.1";
	m_serverConfigInfo.m_localAddress.m_nServerPort = 8801;
	return true;
}

CGateServerConfig::SConfigInfo& CGateServerConfig::getConfigInfo()
{
	return m_serverConfigInfo;
}
