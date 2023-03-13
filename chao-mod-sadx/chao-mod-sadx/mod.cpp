#include "pch.h"
#include "network.h"
#include "chaomachinehacks.h"

HelperFunctions HelperFunctionsGlobal;
std::string modpath;

Network network = Network("SADX");
ChaoHacks chaohacks = ChaoHacks(&network);


extern "C"
{
	__declspec(dllexport) void __cdecl Init(const char* path, const HelperFunctions& helper_funcs)
	{
		HelperFunctionsGlobal = helper_funcs; // Save the helper pointer for external use
		modpath = path;

		network.setupServer();

		//ChaoHacks chaohacks = ChaoHacks(&network);
	}

	__declspec(dllexport) void __cdecl OnFrame()
	{
		if (network.getIsConnected())
		{
			WriteData<1>((void*)0x720B07, 3);
		}
		else
		{
			WriteData<1>((void*)0x720B07, 0);
		}
	}

	__declspec(dllexport) void __cdecl OnInput()
	{
		/*if (Controllers[0].PressedButtons & Buttons_Y && Controllers[0].HeldButtons & Buttons_Left)
		{
			if (serverThread.joinable())
				serverThread.join();

			printf("started broadcaster\n");

			serverThread = std::thread(&Network::runBroadcaster, &network);
		}*/
	}

	__declspec(dllexport) void __cdecl OnExit()
	{
		printf("exiting\n");
		network.cleanupNetwork();

		if (chaohacks.serverThread.joinable())
			chaohacks.serverThread.join();
	}

	__declspec(dllexport) ModInfo SADXModInfo = { ModLoaderVer };
}
