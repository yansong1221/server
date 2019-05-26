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


	//���ķ���������������Ϣ
	void onMeesageFromCenterServer(CNetPacket* pNetPacket);

	void onMeesageFromGateServer(int nConnID, CNetPacket* pNetPacket);
	//ע�����ķ��������ص���Ϣ
	void registerCenterServerMessage(uint16_t nMainCmd, uint16_t nSubCmd, CenterServerMessageCallBacker callBack);
	void registerGateServerMessage(uint16_t nMainCmd, uint16_t nSubCmd, GateServerMessageCallBacker callBack);

	// ��������ע������ķ�����
	void registerServerToCenterServer();
	void connectCenterServer();
	void reconnectCenterServer();

	void registerAllMessage();

	void tryConnectAllGateServer();

	bool startUp();
	void run();

	//------------------------------------���ķ�������Ϣ����------------------------------------//	
	//�����ķ�����ע����
	void onCenterServerRegisterResult(CNetPacket* pNetPacket);
	//���ķ��������ص����ط������б�
	void OnCenterServerGateServerList(CNetPacket* pNetPacket);
	//�����ض���
	void onCenterServerGateOffline(CNetPacket* pNetPacket);
	//����������
	void onCenterServerGateOnline(CNetPacket* pNetPacket);

	//-----------------------------------���ط�������Ϣ���� ----------------------------------//
	void onGateServerRegisterResult(int nConnID, CNetPacket* pNetPacket);
private:
	CEventDispatcher m_eventDispatcher;
	CTCPManager	m_tcpManager;

	CTCPSeesion* m_pCenterServerSeesion;

	std::unordered_map<int32_t, std::list<CenterServerMessageCallBacker>> m_mapCenterServerMessageCallBack;
	std::unordered_map<int32_t, std::list<GateServerMessageCallBacker>> m_mapGateServerMessageCallBack;

	int32_t m_nServerID;
};