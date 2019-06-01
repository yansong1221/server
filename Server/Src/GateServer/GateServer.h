#pragma once

#include "../CoreService/TCPManager.h"
#include "../CoreService/EventDispatcher.h"
#include "CenterServerManager.h"
#include "ClientAgentManager.h"

#include <unordered_map>

using GateServerMessageCallBacker = std::function<void(int,CNetPacket*)>;

class CCenterServerManager;
class CClientAgentManager;

class CGateServer : ITCPEventDelegate
{
public:
	CGateServer();
	~CGateServer();

protected:
	virtual void onNetworkMessage(uint32_t nConnID, CNetPacket* pNetPacket);
	virtual void onConnected(uint32_t nConnID);
	virtual void onDisconnected(uint32_t nConnID);

	//返回false表示不是网关直接处理的消息
	bool onMessageGateServerProcess(uint32_t nConnID, CNetPacket* pNetPacket);

	//注册网关直接处理消息
	void registerMessage(uint16_t nSubCmd, GateServerMessageCallBacker callBack);

	bool listenLocalAddress();
	void registerAllMessage();

	//-----------------------------------网关直接处理消息------------------------------------//	
	void onGateServerRegisterServer(uint32_t nConnID, CNetPacket* pNetPacket);
	void onRelayToClientMessage(uint32_t nConnID, CNetPacket* pNetPacket);
	void onHeartBeatMessage(uint32_t nConnID, CNetPacket* pNetPacket);

public:
	bool startUp();
	void run();

private:
	CEventDispatcher m_eventDispatcher;
	CTCPManager m_TCPManager;
	CCenterServerManager m_CenterServerManager;
	CClientAgentManager m_ClientAgentManager;

	std::unordered_map<int32_t, std::list<GateServerMessageCallBacker>> m_mapGateServerMessageCallBack;
};