#pragma once

#include <boost/serialization/singleton.hpp>
#include <cstdint>
#include <functional>
#include <list>
#include <unordered_map>

class CTCPManager;
class CTCPSeesion;
class CNetPacket;

using CenterServerMessageCallBacker = std::function<void(CNetPacket*)>;

class CCenterServerManager
{
public:
	CCenterServerManager(CTCPManager& TCPManager);
	~CCenterServerManager();
public:

	
	void connectCenterServer();
	void reconnectCenterServer();

	void registerServerToCenterServer();

	void sendMessage(CNetPacket* pNetPacket);
	
	uint32_t getConnID() const;
	//注册所有中心服务器消息
	void registerAllMessage();
	//中心服务器发过来的消息
	void invokeMessage(CNetPacket* pNetPacket);
	//注册中心服务器返回的消息
	void registerMessage(uint16_t nSubCmd, CenterServerMessageCallBacker callBack);
public:

	//------------------------------------中心服务器消息处理------------------------------------//		
	void onCenterServerRegisterResult(CNetPacket* pNetPacket);

private:
	CTCPSeesion* m_pTCPSeesion;
	CTCPManager& m_TCPManager;

	std::unordered_map<int32_t, std::list<CenterServerMessageCallBacker>> m_mapCenterServerMessageCallBack;
	uint32_t m_nServerID;
	
};

