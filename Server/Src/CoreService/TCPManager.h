#pragma once

#include "TCPSeesion.h"
#include <boost/asio.hpp>
#include "NetPacket.h"

class ITCPEventDelegate
{
public:
	virtual void onNetworkMessage(uint32_t nConnID, CNetPacket* pNetPacket) = 0;
	virtual void onConnected(uint32_t nConnID) = 0;
	virtual void onDisconnected(uint32_t nConnID) = 0;
};

class CEventDispatcher;
class CTCPSeesion;
class CTCPManager
{
	friend class CTCPSeesionManager;
public:
	CTCPManager(CEventDispatcher& EventDispatcher,ITCPEventDelegate& pTCPEventDelegate, int nMaxConnNum = 1024);
	~CTCPManager();

	void setAddress(const std::string& szIPAddress, int nPort);
	void listen(int nBacklog = 512);

	void startAccept();

	CTCPSeesion* connectTo(const std::string& szIPAddress, int nPort);
	void asyncConnectTo(const std::string& szIPAddress, int nPort,std::function<void(CTCPSeesion*)> connHandler);

	void closeSeesion(uint32_t nConnID);

	void sendData(uint32_t nConnID, CNetPacket* pNetPacket);

	std::string getListenAddress() const;
	int getListenPort() const;

	CTCPSeesion* findTCPSeesionByConnID(uint32_t nConnID);

	CEventDispatcher& getEventDispatcher();
private:
	CEventDispatcher& m_EventDispatcher;
	boost::asio::ip::tcp::acceptor m_Acceptor;
	boost::asio::ip::tcp::endpoint m_BindEndpoint;

	ITCPEventDelegate& m_pTCPEventDelegate;
	CTCPSeesionManager	m_TCPSeesionManager;

};

