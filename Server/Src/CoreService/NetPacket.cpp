#include "NetPacket.h"



CNetPacket::CNetPacket()
{
}


CNetPacket::~CNetPacket()
{
}

int CNetPacket::parse(const void* pData, int nSize)
{
	reset();

	if (nSize < sizeof(SMsgHeader))
	{
		return 0;
	}

	auto pMsgHeader = reinterpret_cast<const SMsgHeader*>(pData);

	if (pMsgHeader->unLen > MAX_DATA_LEN)
	{
		throw std::runtime_error("数据过长");
	}

	//现有数据不够一个整包
	if (((int)pMsgHeader->unLen + (int)sizeof(SMsgHeader)) > nSize)
	{
		return 0;
	}

	m_MsgHeader.unLen = pMsgHeader->unLen;
	m_MsgHeader.unMainCmd = pMsgHeader->unMainCmd;
	m_MsgHeader.unSubCmd = pMsgHeader->unSubCmd;

	if (pMsgHeader->unLen > 0)
	{
		m_Body.appendBinary((const char*)pData + sizeof(SMsgHeader), pMsgHeader->unLen);
	}
	return pMsgHeader->unLen + sizeof(SMsgHeader);
}

std::string CNetPacket::getPackingData()
{
	std::string tempBuffer;
	m_MsgHeader.unLen = m_Body.size();
	tempBuffer.append((const char*)&m_MsgHeader, sizeof(m_MsgHeader));
	if (m_Body.empty() == false) 
	{
		tempBuffer.append((const char*)m_Body.data(), m_Body.size());
	}
	return tempBuffer;
}

SMsgHeader& CNetPacket::getMsgHeader()
{
	return m_MsgHeader;
}

CMemoryStream& CNetPacket::getBody()
{
	return m_Body;
}

void CNetPacket::reset()
{
	m_Body.clear();
	memset(&m_MsgHeader, 0, sizeof(m_MsgHeader));
}
