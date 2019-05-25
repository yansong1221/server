#pragma once
#pragma pack(1)

enum EMessageMainCmd
{
	eMessageCenterServer = 1,
};
enum EMessageSubCenterServer
{
	eMessageRegisterServer = 1,
	eMessageRegisterServerResult,
};

struct CMDRegisterServer
{
	enum EServerType
	{
		eServerTypeGateServer = 1,
	};

	EServerType eServerType;		//����������
	char szServerAddress[32];		//��������ַ
	int32_t n32Port;				//�������˿�
};

#pragma pack()