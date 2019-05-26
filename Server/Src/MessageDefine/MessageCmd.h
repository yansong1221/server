#pragma once
#pragma pack(1)
namespace CMD
{
	enum class EMainCmd
	{
		eMessageCenterServer = 1,
		eMessageGateServer = 2,
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
		eMessageRegisterServerResult
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

			EServerType eServerType;		//����������
			char szServerAddress[32];		//��������ַ
			int32_t n32Port;				//�������˿�
		};

		struct RegisterServerResult
		{
			bool bSuccess;					//ע���Ƿ�ɹ�
			int32_t nServerID;				//���ķ����������ServreID
		};

		struct GateServerInfo
		{
			char szServerAddress[32];		//��������ַ
			int32_t n32Port;				//�������˿�
			int32_t n32ServerID;			//������ID
		};

		//��������
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
	}
}

#pragma pack()