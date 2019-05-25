#pragma once

#include <cstdint>
#include "MemoryStream.h"

#pragma pack(1)

struct SMsgHeader
{
	uint16_t unMainCmd;
	uint16_t unSubCmd;
	uint16_t unLen;
};

class CNetPacket
{
public:
	CNetPacket();
	~CNetPacket();

	//返回解析成功的字节数，为0说明字节数不够，解包出错抛出运行时异常
	int parse(const void* pData, int nSize);

	std::string getPackingData();

	SMsgHeader& getMsgHeader();
	CMemoryStream& getBody();
private:
	SMsgHeader m_MsgHeader;
	CMemoryStream m_Body;

public:
	//最大支持的字节数
	const static int MAX_DATA_LEN = 1024;
};

#pragma pack()

