#pragma once

#include "../CoreService/TCPManager.h"
#include "../CoreService/EventDispatcher.h"
#include "../CoreService/NetPacket.h"

#include <unordered_map>

using CenterServerMessageCallBacker = std::function<void(CNetPacket*)>;
using GateServerMessageCallBacker = std::function<void(int,CNetPacket*)>;

class CGateServer : ITCPEventDelegate
{
public:
	CGateServer();
	~CGateServer();

protected:
	virtual void onNetworkMessage(int nConnID, CNetPacket* pNetPacket);
	virtual void onConnected(int nConnID);
	virtual void onDisconnected(int nConnID);

	//重新连接中心服务器
	void reconnectCenterServer();
	void connectCenterServer();

	//将次网关注册给中心服务器
	void registerServerToCenterServer();
	//中心服务器发过来的消息
	void onMeesageFromCenterServer(CNetPacket* pNetPacket);

	//返回false表示不是网关直接处理的消息
	bool onMessageGateServer(int nConnID, CNetPacket* pNetPacket);

	//注册中心服务器返回的消息
	void registerCenterServerMessage(uint16_t nMainCmd, uint16_t nSubCmd, CenterServerMessageCallBacker callBack);
	//注册网关直接处理消息
	void registerGateServerMessage(uint16_t nMainCmd, uint16_t nSubCmd, GateServerMessageCallBacker callBack);

	bool listenLocalAddress();
	void registerAllMessage();

	void sendMessageToCenterServer(CNetPacket* pNetPacket);

	//------------------------------------中心服务器消息处理------------------------------------//		
	void onCenterServerRegisterResult(CNetPacket* pNetPacket);

	//-----------------------------------网关直接处理消息------------------------------------//	
	void onGateServerRegisterServer(int nConnID, CNetPacket* pNetPacket);

public:
	bool startUp();
	void run();

private:
	CEventDispatcher m_eventDispatcher;
	CTCPManager m_tcpManager;
	CTCPSeesion* m_pCenterServerSeesion;

	std::unordered_map<int32_t, std::list<CenterServerMessageCallBacker>> m_mapCenterServerMessageCallBack;
	std::unordered_map<int32_t, std::list<GateServerMessageCallBacker>> m_mapGateServerMessageCallBack;
};