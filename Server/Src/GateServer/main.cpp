#include "GateServer.h"

int main()
{
	CGateServer sev;
	if (sev.startUp() == false)
	{
		getchar();
		return -1;
	}
	sev.run();
	return 0;
}