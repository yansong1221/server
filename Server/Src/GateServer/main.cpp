#include "GateServer.h"

int main()
{
	CGateServer sev;
	if (sev.startUp() == false)
	{

	}
	sev.run();
	return 0;
}