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
	//ע���������ķ�������Ϣ
	void registerAllMessage();
	//���ķ���������������Ϣ
	void invokeMessage(CNetPacket* pNetPacket);
	//ע�����ķ��������ص���Ϣ
	void registerMessage(uint16_t nSubCmd, CenterServerMessageCallBacker callBack);
public:

	//------------------------------------���ķ�������Ϣ����------------------------------------//		
	void onCenterServerRegisterResult(CNetPacket* pNetPacket);

private:
	CTCPSeesion* m_pTCPSeesion;
	CTCPManager& m_TCPManager;

	std::unordered_map<int32_t, std::list<CenterServerMessageCallBacker>> m_mapCenterServerMessageCallBack;
	uint32_t m_nServerID;
	
};

