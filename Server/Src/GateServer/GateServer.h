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

	//����false��ʾ��������ֱ�Ӵ������Ϣ
	bool onMessageGateServerProcess(uint32_t nConnID, CNetPacket* pNetPacket);

	//ע������ֱ�Ӵ�����Ϣ
	void registerMessage(uint16_t nSubCmd, GateServerMessageCallBacker callBack);

	bool listenLocalAddress();
	void registerAllMessage();

	//-----------------------------------����ֱ�Ӵ�����Ϣ------------------------------------//	
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