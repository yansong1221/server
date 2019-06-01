#include "GateServer.h"
#include "../CoreService/LogService.h"
#include "../MessageDefine/MessageCmd.h"
#include "GateServerConfig.h"
#include "fmt/core.h"
#include "LoginServerManager.h"
#include "CenterServerManager.h"

CGateServer::CGateServer()
	:m_TCPManager(m_eventDispatcher,*this),
	m_CenterServerManager(m_TCPManager),
	m_ClientAgentManager(m_TCPManager)
{

}

CGateServer::~CGateServer()
{

}

void CGateServer::onNetworkMessage(uint32_t nConnID, CNetPacket* pNetPacket)
{
	//中心服务器消息
	if (m_CenterServerManager.getConnID() == nConnID)
	{
		return  m_CenterServerManager.invokeMessage(pNetPacket);
	}

	//网关直接处理消息 //或者其他服务器转发给客户端的消息
	if (onMessageGateServerProcess(nConnID, pNetPacket))
	{
		return;
	}

	//客户端消息转发
	m_ClientAgentManager.onClientRelayToServerMessage(nConnID, pNetPacket);
}

void CGateServer::onConnected(uint32_t nConnID)
{

}

void CGateServer::onDisconnected(uint32_t nConnID)
{
	//中心服务器断开连接
	if (m_CenterServerManager.getConnID() == nConnID)
	{
		m_CenterServerManager.reconnectCenterServer();
		return;
	}

	//登陆服务器断开连接
	auto pLoginServer = CLoginServerManager::get_mutable_instance().findLoginServerByConnID(nConnID);
	if (pLoginServer)
	{

		CLogService::get_mutable_instance().logWarning(fmt::format("登陆服务器断开连接,ConnID:{},ServerID:{}",
			pLoginServer->getConnID(),
			pLoginServer->getServerID()));

		CLoginServerManager::get_mutable_instance().serverDisconnect(nConnID);

		return;
	}
	
}

bool CGateServer::onMessageGateServerProcess(uint32_t nConnID, CNetPacket* pNetPacket)
{
	SMsgHeader& msgHeader = pNetPacket->getMsgHeader();
	int32_t nMessageID = int32_t(msgHeader.unMainCmd << 16) | msgHeader.unSubCmd;

	auto iter = m_mapGateServerMessageCallBack.find(nMessageID);
	if (iter == m_mapGateServerMessageCallBack.end())
	{
		return false;
	}
	for (auto callBack : iter->second)
	{
		callBack(nConnID,pNetPacket);
	}
	return true;
}


void CGateServer::registerMessage(uint16_t nSubCmd, GateServerMessageCallBacker callBack)
{
	
	int32_t nMessageID = int32_t((uint16_t)CMD::EMainCmd::eMessageGateServer << 16) | nSubCmd;

	auto iter = m_mapGateServerMessageCallBack.find(nMessageID);
	if (iter == m_mapGateServerMessageCallBack.end())
	{
		std::list<GateServerMessageCallBacker> listMessageCallBack;
		listMessageCallBack.push_back(callBack);
		m_mapGateServerMessageCallBack[nMessageID] = listMessageCallBack;
		return;
	}
	iter->second.push_back(callBack);
}

bool CGateServer::listenLocalAddress()
{
	try
	{
		//监听本地端口
		m_TCPManager.setAddress(
			CGateServerConfig::get_mutable_instance().getConfigInfo().m_localAddress.m_szServerAddress,
			CGateServerConfig::get_mutable_instance().getConfigInfo().m_localAddress.m_nServerPort
		);

		m_TCPManager.listen();
		m_TCPManager.startAccept();
	}
	catch (const std::exception& e)
	{
		CLogService::get_mutable_instance().logInfo(fmt::format("监听本地端口[{}:{}]失败!错误消息:{}", 
			CGateServerConfig::get_mutable_instance().getConfigInfo().m_localAddress.m_szServerAddress,
			CGateServerConfig::get_mutable_instance().getConfigInfo().m_localAddress.m_nServerPort, 
			e.what()));
		return false;
	}

	return true;
}

void CGateServer::registerAllMessage()
{
	m_CenterServerManager.registerAllMessage();

	//注册服务器
	registerMessage((uint16_t)CMD::ESubGateCmd::eMessageRegisterServer, std::bind(&CGateServer::onGateServerRegisterServer, this, std::placeholders::_1, std::placeholders::_2));
	//服务器转发给客户端的消息处理
	registerMessage((uint16_t)CMD::ESubGateCmd::eMessageRelay, std::bind(&CGateServer::onRelayToClientMessage, this, std::placeholders::_1, std::placeholders::_2));
	//心跳
	registerMessage((uint16_t)CMD::ESubGateCmd::eMessageHeartBeat, std::bind(&CGateServer::onHeartBeatMessage, this, std::placeholders::_1, std::placeholders::_2));
}

void CGateServer::onGateServerRegisterServer(uint32_t nConnID, CNetPacket* pNetPacket)
{
	try
	{
		CMD::GateServer::RegisterServer registerServer;
		pNetPacket->getBody() >> registerServer;

		//登录服务器注册
		if (registerServer.eServerType == CMD::GateServer::RegisterServer::eServerTypeLoginServer)
		{
			auto pLoginServer = CLoginServerManager::get_mutable_instance().addLoginServer(registerServer.nServerID, m_TCPManager.findTCPSeesionByConnID(nConnID));

			CMD::GateServer::RegisterServerResult registerServerResult;
			registerServerResult.bSuccess = (pLoginServer != nullptr);

			CNetPacket packet;

			packet.getMsgHeader().unMainCmd = (uint16_t)CMD::EMainCmd::eMessageGateServer;
			packet.getMsgHeader().unSubCmd = (uint16_t)CMD::ESubGateCmd::eMessageRegisterServerResult;

			packet.getBody() << registerServerResult;

			m_TCPManager.sendData(nConnID, &packet);
		}

	}
	catch (const std::exception&)
	{

	}
}


void CGateServer::onRelayToClientMessage(uint32_t nConnID, CNetPacket* pNetPacket)
{
	m_ClientAgentManager.onServerRelayToClientMessage(nConnID, pNetPacket);
}

void CGateServer::onHeartBeatMessage(uint32_t nConnID, CNetPacket* pNetPacket)
{
	auto pTCPSeesion = m_TCPManager.findTCPSeesionByConnID(nConnID);

	if (pTCPSeesion == nullptr)
	{
		assert(false);
		return;
	}
}

bool CGateServer::startUp()
{
	CLOG_INFO("正在启动服务器...");

	if (CGateServerConfig::get_mutable_instance().loadConfig() == false)
	{
		CLOG_FATAL("载入服务器配置失败！");
		return false;
	}

	if (listenLocalAddress() == false)
	{
		return false;
	}
	registerAllMessage();
	m_CenterServerManager.connectCenterServer();
	
	CLOG_INFO("服务器启动成功！");
	return true;
}

void CGateServer::run()
{
	m_eventDispatcher.run();
}

