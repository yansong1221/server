#pragma once

#include <boost/asio.hpp>
#include "Timer.h"

class CEventDispatcher
{
public:
	CEventDispatcher();
	~CEventDispatcher();

	boost::asio::io_service& getIOService();
	CTimerManager& getTimerManager();
public:
	void run();
private:
	boost::asio::io_service m_IOService;
	volatile bool m_bRunStatus;

	CTimerManager m_timerManager;
};

