#include "TCPManager.h"
#include "EventDispatcher.h"


CTCPManager::CTCPManager(CEventDispatcher& EventDispatcher, ITCPEventDelegate& pTCPEventDelegate,int nMaxConnNum)
	:m_EventDispatcher(EventDispatcher),
	m_Acceptor(m_EventDispatcher.getIOService()),
	m_BindEndpoint(boost::asio::ip::address_v4::any(),INADDR_ANY),
	m_pTCPEventDelegate(pTCPEventDelegate),
	m_TCPSeesionManager(nMaxConnNum, *this)
{

}


CTCPManager::~CTCPManager()
{
}

void CTCPManager::setAddress(const std::string& szIPAddress, int nPort)
{
	m_BindEndpoint = boost::asio::ip::tcp::endpoint(boost::asio::ip::address_v4::from_string(szIPAddress), nPort);
}

void CTCPManager::listen(int nBacklog)
{
	m_Acceptor.open(m_BindEndpoint.protocol());
	m_Acceptor.bind(m_BindEndpoint);
	m_Acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(false));
	m_Acceptor.listen(nBacklog);	
}

void CTCPManager::startAccept()
{
	 auto pTCPSeesion =  m_TCPSeesionManager.createSeesion();
	m_Acceptor.async_accept(pTCPSeesion->getScoket(), [this,pTCPSeesion](const boost::system::error_code ec)
	{
		if (ec)
		{
			//出现异常情况
			return;
		}
		pTCPSeesion->setConnectOk();
		m_pTCPEventDelegate.onConnected(pTCPSeesion->getConnID());
		startAccept();
	});
}

CTCPSeesion* CTCPManager::connectTo(const std::string& szIPAddress, int nPort)
{
	auto pTCPSeesion = m_TCPSeesionManager.createSeesion();
	auto connEndpoint = boost::asio::ip::tcp::endpoint(boost::asio::ip::address_v4::from_string(szIPAddress), nPort);

	boost::system::error_code ec;
	pTCPSeesion->getScoket().connect(connEndpoint, ec);

	if (ec)
	{
		return nullptr;
	}
	pTCPSeesion->setConnectOk();
	return pTCPSeesion;
}

void CTCPManager::asyncConnectTo(const std::string& szIPAddress, int nPort, std::function<void(CTCPSeesion*)> connHandler)
{
	auto pTCPSeesion = m_TCPSeesionManager.createSeesion();
	auto connEndpoint = boost::asio::ip::tcp::endpoint(boost::asio::ip::address_v4::from_string(szIPAddress), nPort);

	pTCPSeesion->getScoket().async_connect(connEndpoint, [this, pTCPSeesion, connHandler](boost::system::error_code ec)
	{
		if (ec)
		{
			connHandler(nullptr);
			return;
		}
		pTCPSeesion->setConnectOk();
		connHandler(pTCPSeesion);
	});
}

void CTCPManager::closeSeesion(uint32_t nConnID)
{
	m_TCPSeesionManager.closeSeesion(nConnID);
}

void CTCPManager::sendData(uint32_t nConnID, CNetPacket* pNetPacket)
{
	auto pTCPSeesion = m_TCPSeesionManager.findSeesionByConnID(nConnID);
	if(pTCPSeesion)pTCPSeesion->sendData(pNetPacket);
}

std::string CTCPManager::getListenAddress() const
{
	try
	{
		return m_Acceptor.local_endpoint().address().to_string();
	}
	catch (const std::exception&)
	{
		return std::string();
	}
}

int CTCPManager::getListenPort() const
{
	try
	{
		return m_Acceptor.local_endpoint().port();
	}
	catch (const std::exception&)
	{
		return 0;
	}
	
}

CTCPSeesion* CTCPManager::findTCPSeesionByConnID(uint32_t nConnID)
{
	return m_TCPSeesionManager.findSeesionByConnID(nConnID);
}

CEventDispatcher& CTCPManager::getEventDispatcher()
{
	return m_EventDispatcher;
}

