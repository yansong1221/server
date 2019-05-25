#pragma once

#include "../CoreService/TCPManager.h"
#include "../CoreService/EventDispatcher.h"
#include "../CoreService/NetPacket.h"

#include <unordered_map>

using CenterServerMessageCallBacker = std::function<void(CNetPacket*)>;

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
	//��������ע������ķ�����
	void registerServerToCenterServer();
	//���ķ���������������Ϣ
	void onMeesageFromCenterServer(CNetPacket* pNetPacket);
	//ע�����ķ��������ص���Ϣ
	void registerCenterServerMsseage(uint16_t nMainCmd, uint16_t nSubCmd, std::function<void(CNetPacket*)> callBack);


	//------------------------------------���ķ�������Ϣ����------------------------------------//
		
	void onCenterServerRegisterResult(CNetPacket* pNetPacket);

public:
	bool startUp();
	void run();

private:
	CEventDispatcher m_eventDispatcher;
	CTCPManager m_tcpManager;
	CTCPSeesion* m_pCenterServerSeesion;

	std::unordered_map<int32_t, std::list<CenterServerMessageCallBacker>> m_mapCenterServerMessageCallBack;
};