#include "EventPoller.h"
#include "IOCPPoller.h"
EventPoller* EventPoller::createEventPoller()
{
	return new IOCPPoller;
}
