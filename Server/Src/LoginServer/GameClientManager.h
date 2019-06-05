#pragma once
#include <unordered_map>
#include <cstdint>
#include <functional>

class CTCPManager;
class CTCPSeesion;
class CNetPacket;

using GameClientMessageHandler = std::function<void(uint32_t, uint32_t, CNetPacket*)>;

class CGameClientManager
{
public:
	CGameClientManager();
	~CGameClientManager();
private:
	std::unordered_map<uint32_t, GameClientMessageHandler> m_mapGameClientMessage;
};

