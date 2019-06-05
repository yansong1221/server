#include "TCPSeesion.h"
#include "TCPManager.h"
#include "EventDispatcher.h"


CTCPSeesion::CTCPSeesion(CTCPSeesionManager& TCPSeesionManager, int nConnID,int nReadBufferSize)
	:m_pSocket(TCPSeesionManager.getIOService()),
	m_TCPSeesionManager(TCPSeesionManager),
	m_nConnID(nConnID),
	m_bConnected(false)
{
	m_TempReadBuffer.resize(nReadBufferSize);
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
		assert(false);
		return;
	}

	m_pSocket.async_read_some(boost::asio::buffer(m_TempReadBuffer), [this](const boost::system::error_code& ec, std::size_t nSize)
	{
		if (ec || nSize == 0)
		{
			clear();
			m_TCPSeesionManager.getTCPEventDelegate().onDisconnected(getConnID());		
			return;
		}
		m_LastRecvTimePoint = std::chrono::steady_clock::now();
		m_ReadBuffer.appendBinary(m_TempReadBuffer.data(), nSize);

		try
		{
			int nSize = 0;
			while ((nSize = m_NetPacket.parse(m_ReadBuffer.data(), m_ReadBuffer.size())) > 0)
			{

				m_TCPSeesionManager.getTCPEventDelegate().onNetworkMessage(getConnID(), &m_NetPacket);
				m_ReadBuffer.remove(0, nSize);
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

void CTCPSeesion::sendData(CNetPacket* pNetPacket /*= nullptr*/)
{
	if (m_pSocket.is_open() == false && isConnectOk() == false)
	{
		return;
	}

	if (pNetPacket)
	{
		std::string sendBuffer = pNetPacket->getPackingData();
		m_SendBuffer.append(sendBuffer);

	}
	
	if (!m_SenddingBuffer.empty() || m_SendBuffer.empty())
	{
		return;
	}

	m_SenddingBuffer = m_SendBuffer;
	m_SendBuffer.clear();

	m_pSocket.async_write_some(boost::asio::buffer(m_SenddingBuffer), [this](const boost::system::error_code& ec, std::size_t nSize)
	{
		if (ec)
		{
			//发送发生错误可以忽略，因为无论怎么结束结束一定也会发生错误
			m_SenddingBuffer.clear();
			m_SendBuffer.clear();
			return;
		}
		if (nSize == m_SenddingBuffer.size())
		{
			m_SenddingBuffer.clear();
			sendData();
		}
	});
}

void CTCPSeesion::setConnectOk()
{
	m_bConnected = true;
	startRecv();
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
