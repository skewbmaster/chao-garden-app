#include "pch.h"
#include "network.h"

void doThread(int num)
{
	for (int i = 0; i < num; i++)
	{
		PrintDebug(std::to_string(i).c_str());
	}
}

void hostServer()
{
	std::thread yep(doThread, 60);

	yep.join();
}