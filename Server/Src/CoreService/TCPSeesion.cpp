#include "TCPSeesion.h"
#include "TCPManager.h"
#include "EventDispatcher.h"
#include "NetPacket.h"

CTCPSeesion::CTCPSeesion(CTCPSeesionManager& TCPSeesionManager, int nConnID,int nReadBufferSize)
	:m_pSocket(TCPSeesionManager.getIOService()),
	m_TCPSeesionManager(TCPSeesionManager),
	m_nConnID(nConnID),
	m_bConnected(false)
{
	m_tempReadBuffer.resize(nReadBufferSize);
}

CTCPSeesion::~CTCPSeesion()
{
}

boost::asio::ip::tcp::socket& CTCPSeesion::getScoket()
{
	return m_pSocket;
}

int CTCPSeesion::getConnID() const
{
	return m_nConnID;
}

void CTCPSeesion::close()
{
	m_pSocket.shutdown(boost::asio::socket_base::shutdown_both);
	m_pSocket.close();
	clear();
}

void CTCPSeesion::shutdown()
{
	m_pSocket.shutdown(boost::asio::socket_base::shutdown_both);
	clear();
}

void CTCPSeesion::startRecv()
{
	if (m_pSocket.is_open() == false && isConnectOk() == false)
	{
		return;
	}

	m_pSocket.async_read_some(boost::asio::buffer(m_tempReadBuffer), [this](const boost::system::error_code& ec, std::size_t nSize)
	{
		if (ec || nSize == 0)
		{
			clear();
			m_TCPSeesionManager.getTCPEventDelegate().onDisconnected(getConnID());		
			return;
		}
		m_LastRecvTimePoint = std::chrono::steady_clock::now();
		m_readBuffer.appendBinary(m_tempReadBuffer.data(), nSize);

		CNetPacket packet;
		try
		{
			int nSize = 0;
			while ((nSize = packet.parse(m_readBuffer.data(), m_readBuffer.size())) > 0)
			{

				m_TCPSeesionManager.getTCPEventDelegate().onNetworkMessage(getConnID(), &packet);
				m_readBuffer.remove(0, nSize);
			}
		}
		catch (const std::exception&)
		{
			close();
			return;
		}

		startRecv();
	});
}

void CTCPSeesion::sendData(const void* pData, int nSize)
{
	if (m_pSocket.is_open() == false && isConnectOk() == false)
	{
		return;
	}

	if (pData != nullptr && nSize != 0)
	{
		m_sendBuffer.append((const char*)pData, nSize);
	}

	if (!m_senddingBuffer.empty() || m_sendBuffer.empty())
	{
		return;
	}

	m_senddingBuffer = m_sendBuffer;
	m_sendBuffer.clear();

	m_pSocket.async_write_some(boost::asio::buffer(m_senddingBuffer), [this](const boost::system::error_code& ec, std::size_t nSize)
	{
		if (ec)
		{
			//发送发生错误可以忽略，因为无论怎么结束结束一定也会发生错误
			m_senddingBuffer.clear();
			m_sendBuffer.clear();
			return;
		}
		if (nSize == m_senddingBuffer.size())
		{
			m_senddingBuffer.clear();
			sendData(nullptr, 0);
		}
	});
}

void CTCPSeesion::setConnectOk()
{
	m_bConnected = true;
}

bool CTCPSeesion::isConnectOk() const
{
	return m_bConnected;
}

void CTCPSeesion::clear()
{
	m_bConnected = false;
}

CTCPSeesionManager::CTCPSeesionManager(uint16_t nMaxSeesionNum, CTCPManager& TCPManager)
	:m_TCPManager(TCPManager),
	m_nNextArrayIndex(0)
{
	for (int nArrayIndex = 0; nArrayIndex < nMaxSeesionNum; ++nArrayIndex)
	{
		m_vecTCPSeesion.push_back(new CTCPSeesion(*this, nArrayIndex + 1));
	}	
}

CTCPSeesionManager::~CTCPSeesionManager()
{
	for (auto pTCPSeesion : m_vecTCPSeesion)
	{
		delete pTCPSeesion;
	}
	m_vecTCPSeesion.clear();
}

CTCPSeesion* CTCPSeesionManager::createSeesion()
{
	size_t findCount = 0;
	do 
	{
		m_nNextArrayIndex = (m_nNextArrayIndex + 1) % m_vecTCPSeesion.size();
		auto pTCPSeesion = m_vecTCPSeesion[m_nNextArrayIndex - 1];
		if (pTCPSeesion->isConnectOk() == false)
		{
			return pTCPSeesion;
		}

		findCount++;

	} while (findCount < m_vecTCPSeesion.size());

	return nullptr;
}

CTCPSeesion* CTCPSeesionManager::findSeesionByConnID(int nConnID)
{
	if (nConnID <= 0)
	{
		return nullptr;
	}

	uint16_t arrayIndex = nConnID - 1;

	if (arrayIndex > m_vecTCPSeesion.size() - 1)
	{
		assert(false);
		return nullptr;
	}

	return m_vecTCPSeesion[arrayIndex];
}

void CTCPSeesionManager::closeSeesion(int nConnID)
{
	auto pTCPSeesion = findSeesionByConnID(nConnID);
	if (pTCPSeesion == nullptr)
	{
		assert(false);
		return;
	}
	pTCPSeesion->close();
}

void CTCPSeesionManager::closeSeesion(CTCPSeesion* pTCPSeesion)
{
	if (pTCPSeesion == nullptr)
	{
		assert(false);
		return;
	}
	pTCPSeesion->close();
}

ITCPEventDelegate& CTCPSeesionManager::getTCPEventDelegate()
{
	return m_TCPManager.m_pTCPEventDelegate;
}

boost::asio::io_service& CTCPSeesionManager::getIOService()
{
	return m_TCPManager.getEventDispatcher().getIOService();
}

uint16_t CTCPSeesionManager::s_bindIndex = 0;