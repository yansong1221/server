#pragma once

#include "../CoreService/TCPManager.h"
#include "../CoreService/EventDispatcher.h"
#include "../CoreService/NetPacket.h"
#include "../MessageDefine/MessageCmd.h"

#include "ServerManager.h"

#include <unordered_map>

using ServerMessageCallBacker = std::function<void(int,CNetPacket*)>;

class CCenterServer : ITCPEventDelegate
{
public:
	CCenterServer();
	~CCenterServer();

public:
	virtual void onNetworkMessage(uint32_t nConnID, CNetPacket* pNetPacket);
	virtual void onConnected(uint32_t nConnID);
	virtual void onDisconnected(uint32_t nConnID);

	
	
	bool listenLocalAddress();

	void registerAllMessage();
	void registerServerMessage(uint16_t unMainCmd, uint16_t unSubCmd, ServerMessageCallBacker callBack);

	void sendGateServerList(uint32_t nConnID);

	//----------------------服务器收到的消息--------------------------//
	void onMessageRegisterServer(uint32_t nConnID, CNetPacket* pNetPacket);
public:

	bool startUp();
	void run();

private:
	CEventDispatcher m_EventDispatcher;
	CTCPManager m_TCPManager;
	CServerManager m_ServerManager;

	std::unordered_map<int32_t, std::list<ServerMessageCallBacker>> m_mapServerMessageCallBack;
};