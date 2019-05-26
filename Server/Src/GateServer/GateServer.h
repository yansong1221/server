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

	//�����������ķ�����
	void reconnectCenterServer();
	void connectCenterServer();

	//��������ע������ķ�����
	void registerServerToCenterServer();
	//���ķ���������������Ϣ
	void onMeesageFromCenterServer(CNetPacket* pNetPacket);

	//����false��ʾ��������ֱ�Ӵ������Ϣ
	bool onMessageGateServer(int nConnID, CNetPacket* pNetPacket);

	//ע�����ķ��������ص���Ϣ
	void registerCenterServerMessage(uint16_t nMainCmd, uint16_t nSubCmd, CenterServerMessageCallBacker callBack);
	//ע������ֱ�Ӵ�����Ϣ
	void registerGateServerMessage(uint16_t nMainCmd, uint16_t nSubCmd, GateServerMessageCallBacker callBack);

	bool listenLocalAddress();
	void registerAllMessage();

	void sendMessageToCenterServer(CNetPacket* pNetPacket);

	//------------------------------------���ķ�������Ϣ����------------------------------------//		
	void onCenterServerRegisterResult(CNetPacket* pNetPacket);

	//-----------------------------------����ֱ�Ӵ�����Ϣ------------------------------------//	
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