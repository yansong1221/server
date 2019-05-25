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

	EServerType eServerType;		//服务器类型
	char szServerAddress[32];		//服务器地址
	int32_t n32Port;				//服务器端口
};

#pragma pack()