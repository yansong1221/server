#include "Timer.h"
#include <algorithm>

CTimerItem::CTimerItem(CTimerManager& timerManager, TimeoutCallBack callBack, time_t elapseTime)
	:m_startTimePoint(std::chrono::steady_clock::now()),
	m_timerManager(timerManager),
	m_timeoutCallBack(callBack),
	m_elapseTime(elapseTime),
	m_bCancel(false)
{

}

CTimerItem::~CTimerItem()
{

}

time_t CTimerItem::remainTime() const
{
	auto nowTimePoint = std::chrono::steady_clock::now();
	auto spanCount =  std::chrono::duration_cast<std::chrono::milliseconds>(nowTimePoint - m_startTimePoint).count();
	
	return std::max(0LL, m_elapseTime - spanCount);
}

bool CTimerItem::isTimeout() const
{
	return remainTime() == 0;
}

void CTimerItem::triggerTimeout()
{
	if (isTimeout() == false || isCancel())
	{
		return;
	}
	m_timeoutCallBack(this);
	m_startTimePoint = std::chrono::steady_clock::now();
}

void CTimerItem::cancelTimer()
{
	m_bCancel = true;
}

bool CTimerItem::isCancel()
{
	return m_bCancel;
}

CTimerManager::CTimerManager()
{
}


CTimerManager::~CTimerManager()
{
}

CTimerItem* CTimerManager::addTimer(time_t elapseTime, TimeoutCallBack callBack)
{
	auto pTimerItem = new CTimerItem(*this, callBack, elapseTime);
	m_queTimerItem.push(pTimerItem);
	return pTimerItem;
}

void CTimerManager::poll()
{
	while (!m_queTimerItem.empty() && m_queTimerItem.top()->isTimeout())
	{
		auto pTimerItem = m_queTimerItem.top();
		m_queTimerItem.pop();

		if (pTimerItem->isCancel() == false)
		{
			pTimerItem->triggerTimeout();
			m_queTimerItem.push(pTimerItem);
		}
		else
		{
			delete pTimerItem;
		}
	}
}


