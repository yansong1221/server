#pragma once

#include "../CoreService/TCPManager.h"
#include "../CoreService/EventDispatcher.h"
#include "../CoreService/NetPacket.h"
#include "../MessageDefine/MessageCmd.h"

#include <unordered_map>

using ServerMessageCallBacker = std::function<void(int,CNetPacket*)>;

class CCenterServer : ITCPEventDelegate
{
public:
	CCenterServer();
	~CCenterServer();

public:
	virtual void onNetworkMessage(int nConnID, CNetPacket* pNetPacket);
	virtual void onConnected(int nConnID);
	virtual void onDisconnected(int nConnID);

	bool startUp();
	
	bool listenLocalAddress();

	void registerAllMessage();
	void registerServerMessage(uint16_t unMainCmd, uint16_t unSubCmd, ServerMessageCallBacker callBack);

	void sendGateServerList(int nConnID);

	void broadcastMessageToLoginServer(CNetPacket* pNetPacket);

	//----------------------服务器收到的消息--------------------------//
	void onMessageRegisterServer(int nConnID, CNetPacket* pNetPacket);
public:
	void run();

private:
	CEventDispatcher m_EventDispatcher;
	CTCPManager m_TCPManager;

	std::unordered_map<int32_t, std::list<ServerMessageCallBacker>> m_mapServerMessageCallBack;
};