
#include "CenterServer.h"

int main()
{
	CCenterServer sev;
	if (sev.startUp() == false)
	{
		getchar();
		return -1;
	}
	sev.run();
	return 0;
}