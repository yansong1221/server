#include "LoginServer.h"

int main()
{
	CLoginServer sev;

	if (sev.startUp() == false)
	{
		getchar();
		return -1;
	}
	sev.run();
	return 0;
}