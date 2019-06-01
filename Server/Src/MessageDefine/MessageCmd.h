#pragma once
#pragma pack(1)
namespace CMD
{
	enum class EMainCmd
	{
		eMessageCenterServer = 1,
		eMessageGateServer = 2,
		eMessageLoginServer = 3,	
	};
	enum class ESubCenterCmd
	{
		eMessageRegisterServer = 1,
		eMessageRegisterServerResult,
		eMessageGateServerOnline,
		eMessageGateServerList,
		eMessageGateOffline,
	};

	enum class ESubGateCmd
	{
		eMessageRegisterServer = 1,
		eMessageRegisterServerResult,
		eMessageRelay,
		eMessageHeartBeat,
	};

	enum class ESubLoginCmd
	{
		
	};

	namespace CenterServer
	{
		struct RegisterServer
		{
			enum EServerType
			{
				eServerTypeGateServer = 1,
				eServerTypeLoginServer,
			};

			EServerType eServerType;		//服务器类型
			char szServerAddress[32];		//服务器地址
			int32_t n32Port;				//服务器端口
		};

		struct RegisterServerResult
		{
			bool bSuccess;					//注册是否成功
			int32_t nServerID;				//中心服务器分配的ServreID
		};

		struct GateServerInfo
		{
			char szServerAddress[32];		//服务器地址
			int32_t n32Port;				//服务器端口
			int32_t n32ServerID;			//服务器ID
		};

		//网关离线
		struct GateServerOffline
		{
			int32_t nServerID;
		};

	}

	namespace GateServer
	{
		struct RegisterServer
		{
			enum EServerType
			{
				eServerTypeLoginServer,
			};
			EServerType eServerType;
			int32_t nServerID;
		};

		struct RegisterServerResult
		{
			bool bSuccess;
		};

		struct RelayHeader
		{
			uint32_t nClientConnID;
		};
	}
}

#pragma pack()