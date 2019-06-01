#include "CenterServerManager.h"
#include "../CoreService/LogService.h"
#include <fmt/core.h>
#include "GateServerConfig.h"
#include "../MessageDefine/MessageCmd.h"
#include "../CoreService/TCPManager.h"

CCenterServerManager::CCenterServerManager(CTCPManager& TCPManager)
	:m_pTCPSeesion(nullptr),
	m_TCPManager(TCPManager),
	m_nServerID(0)
{
}


CCenterServerManager::~CCenterServerManager()
{
}

void CCenterServerManager::connectCenterServer()
{
	// �������ķ�����
	while (m_pTCPSeesion == nullptr)
	{
		//�ȴ�2��
		std::this_thread::sleep_for(std::chrono::seconds(2));

		CLOG_INFO(
			fmt::format("�����������ķ�����[{}:{}]...",
				CGateServerConfig::get_mutable_instance().getConfigInfo().m_centerServerAddress.m_szServerAddress,
				CGateServerConfig::get_mutable_instance().getConfigInfo().m_centerServerAddress.m_nServerPort)
		);

		m_pTCPSeesion = m_TCPManager.connectTo(
			CGateServerConfig::get_mutable_instance().getConfigInfo().m_centerServerAddress.m_szServerAddress,
			CGateServerConfig::get_mutable_instance().getConfigInfo().m_centerServerAddress.m_nServerPort
		);	
	}
	CLOG_INFO("���ķ��������ӳɹ���");
	registerServerToCenterServer();
}

void CCenterServerManager::reconnectCenterServer()
{
	if (m_pTCPSeesion && m_pTCPSeesion->isConnectOk())
	{
		return;
	}

	m_nServerID = 0;

	CLOG_INFO("���ڳ��������������ķ�����...");
	m_TCPManager.asyncConnectTo(
		CGateServerConfig::get_mutable_instance().getConfigInfo().m_centerServerAddress.m_szServerAddress,
		CGateServerConfig::get_mutable_instance().getConfigInfo().m_centerServerAddress.m_nServerPort,
		[this](CTCPSeesion* pTCPSeesion)
	{
		if (pTCPSeesion)
		{
			m_pTCPSeesion = pTCPSeesion;
			CLOG_INFO("���ķ����������ɹ���");
			registerServerToCenterServer();
		}
		else
		{
			reconnectCenterServer();
		}
	});
}

void CCenterServerManager::registerServerToCenterServer()
{
	CMD::CenterServer::RegisterServer RegisterServer;
	RegisterServer.eServerType = CMD::CenterServer::RegisterServer::eServerTypeGateServer;
	strcpy(RegisterServer.szServerAddress, m_TCPManager.getListenAddress().c_str());
	RegisterServer.n32Port = m_TCPManager.getListenPort();

	CNetPacket packet;
	packet.getMsgHeader().unMainCmd = (uint16_t)CMD::EMainCmd::eMessageCenterServer;
	packet.getMsgHeader().unSubCmd = (uint16_t)CMD::ESubCenterCmd::eMessageRegisterServer;

	packet.getBody() << RegisterServer;

	sendMessage(&packet);
}

void CCenterServerManager::sendMessage(CNetPacket* pNetPacket)
{
	m_TCPManager.sendData(m_pTCPSeesion->getConnID(), pNetPacket);
}

uint32_t CCenterServerManager::getConnID() const
{
	if (m_pTCPSeesion != nullptr)
	{
		return m_pTCPSeesion->getConnID();
	}
	
	return 0;
}

void CCenterServerManager::registerAllMessage()
{
	//���ķ�������Ϣע��
	registerMessage((uint16_t)CMD::ESubCenterCmd::eMessageRegisterServerResult, std::bind(&CCenterServerManager::onCenterServerRegisterResult, this, std::placeholders::_1));
}

void CCenterServerManager::invokeMessage(CNetPacket* pNetPacket)
{
	SMsgHeader& msgHeader = pNetPacket->getMsgHeader();
	int32_t nMessageID = int32_t(msgHeader.unMainCmd << 16) | msgHeader.unSubCmd;

	auto iter = m_mapCenterServerMessageCallBack.find(nMessageID);
	if (iter == m_mapCenterServerMessageCallBack.end())
	{
		CLOG_WARNING("û��ע�����ķ��������ص���ϢID");
		return;
	}
	for (auto callBack : iter->second)
	{
		callBack(pNetPacket);
	}
	return;
}

void CCenterServerManager::registerMessage(uint16_t nSubCmd, CenterServerMessageCallBacker callBack)
{
	int32_t nMessageID = int32_t((uint16_t)CMD::EMainCmd::eMessageCenterServer << 16) | nSubCmd;

	auto iter = m_mapCenterServerMessageCallBack.find(nMessageID);
	if (iter == m_mapCenterServerMessageCallBack.end())
	{
		std::list<CenterServerMessageCallBacker> listMessageCallBack;
		listMessageCallBack.push_back(callBack);
		m_mapCenterServerMessageCallBack[nMessageID] = listMessageCallBack;
		return;
	}
	iter->second.push_back(callBack);
}

void CCenterServerManager::onCenterServerRegisterResult(CNetPacket* pNetPacket)
{
	try
	{
		CMD::CenterServer::RegisterServerResult registerServerResult;
		pNetPacket->getBody() >> registerServerResult;

		if (registerServerResult.bSuccess)
		{
			CLOG_INFO("ע�����ķ������ɹ�!");
			m_nServerID = registerServerResult.nServerID;
		}
		else
		{
			CLOG_WARNING("ע�����ķ�����ʧ��!");
		}
	}
	catch (const std::exception&)
	{
		CLOG_ERROR("����ʧ��");
	}
}
