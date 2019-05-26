#pragma once

#include "../CoreService/TCPManager.h"
#include "../CoreService/EventDispatcher.h"

#include <unordered_map>

using CenterServerMessageCallBacker = std::function<void(CNetPacket*)>;
using GateServerMessageCallBacker = std::function<void(int, CNetPacket*)>;

class CLoginServer : ITCPEventDelegate
{
public:
	CLoginServer();
	~CLoginServer();
public:
	virtual void onNetworkMessage(int nConnID, CNetPacket* pNetPacket);
	virtual void onConnected(int nConnID);
	virtual void onDisconnected(int nConnID);


	//中心服务器发过来的消息
	void onMeesageFromCenterServer(CNetPacket* pNetPacket);

	void onMeesageFromGateServer(int nConnID, CNetPacket* pNetPacket);
	//注册中心服务器返回的消息
	void registerCenterServerMessage(uint16_t nMainCmd, uint16_t nSubCmd, CenterServerMessageCallBacker callBack);
	void registerGateServerMessage(uint16_t nMainCmd, uint16_t nSubCmd, GateServerMessageCallBacker callBack);

	// 将次网关注册给中心服务器
	void registerServerToCenterServer();
	void connectCenterServer();
	void reconnectCenterServer();

	void registerAllMessage();

	void tryConnectAllGateServer();

	bool startUp();
	void run();

	//------------------------------------中心服务器消息处理------------------------------------//	
	//向中心服务器注册结果
	void onCenterServerRegisterResult(CNetPacket* pNetPacket);
	//中心服务器返回的网关服务器列表
	void OnCenterServerGateServerList(CNetPacket* pNetPacket);
	//有网关断线
	void onCenterServerGateOffline(CNetPacket* pNetPacket);
	//有网关上线
	void onCenterServerGateOnline(CNetPacket* pNetPacket);

	//-----------------------------------网关服务器消息处理 ----------------------------------//
	void onGateServerRegisterResult(int nConnID, CNetPacket* pNetPacket);
private:
	CEventDispatcher m_eventDispatcher;
	CTCPManager	m_tcpManager;

	CTCPSeesion* m_pCenterServerSeesion;

	std::unordered_map<int32_t, std::list<CenterServerMessageCallBacker>> m_mapCenterServerMessageCallBack;
	std::unordered_map<int32_t, std::list<GateServerMessageCallBacker>> m_mapGateServerMessageCallBack;

	int32_t m_nServerID;
};