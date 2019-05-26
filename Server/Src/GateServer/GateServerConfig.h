#pragma once

#include <boost/serialization/singleton.hpp>
#include <string>

class CGateServerConfig : public boost::serialization::singleton<CGateServerConfig>
{

public:
	struct SServerAddressInfo
	{
		std::string	m_szServerAddress;
		int m_nServerPort;
	};
	struct SConfigInfo
	{
		SServerAddressInfo m_localAddress;
		SServerAddressInfo m_centerServerAddress;
	};

public:
	CGateServerConfig();
	~CGateServerConfig();

public:

	bool loadConfig();

	SConfigInfo& getConfigInfo();

protected:
	SConfigInfo m_serverConfigInfo;
};

