#pragma once

#include <cstdint>
#include "../Common/MemoryStream.h"

#pragma pack(1)

struct SMsgHeader
{
	uint16_t unMainCmd;
	uint16_t unSubCmd;
	uint16_t unLen;
};

class NetPacket
{
public:
	NetPacket();
	~NetPacket();

	//���ؽ����ɹ����ֽ�����Ϊ0˵���ֽ�����������������׳�����ʱ�쳣
	int parse(const void* pData, int nSize);

	std::string getPackingData();

	SMsgHeader& getMsgHeader();
	CMemoryStream& getBody();

	void reset();
private:
	SMsgHeader m_MsgHeader;
	CMemoryStream m_Body;

public:
	//���֧�ֵ��ֽ���
	const static int MAX_DATA_LEN = 1024;
};

#pragma pack()
