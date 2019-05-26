#include "EventDispatcher.h"
#include <thread>



CEventDispatcher::CEventDispatcher()
	:m_bRunStatus(true)
{
}


CEventDispatcher::~CEventDispatcher()
{
}

boost::asio::io_service& CEventDispatcher::getIOService()
{
	return m_IOService;
}

CTimerManager& CEventDispatcher::getTimerManager()
{
	return m_timerManager;
}

void CEventDispatcher::run()
{
	while (m_bRunStatus)
	{
		m_timerManager.poll();
		m_IOService.poll();
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}
