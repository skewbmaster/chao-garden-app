#include "pch.h"
#include "network.h"

HelperFunctions HelperFunctionsGlobal;
std::string modpath;

std::thread serverThread;
Network network;

extern "C"
{
	__declspec(dllexport) void __cdecl Init(const char* path, const HelperFunctions& helper_funcs)
	{
		HelperFunctionsGlobal = helper_funcs; // Save the helper pointer for external use
		modpath = path;

		network = Network();
		network.setupServer();
	}

	__declspec(dllexport) void __cdecl OnInput()
	{
		if (Controllers[0].PressedButtons & Buttons_Y && Controllers[0].HeldButtons & Buttons_Left)
		{
			if (serverThread.joinable())
				serverThread.join();

			printf("started broadcaster\n");

			serverThread = std::thread(&Network::runBroadcaster, &network);
		}
	}

	__declspec(dllexport) void __cdecl OnExit()
	{
		printf("exiting\n");
		network.cleanupNetwork();

		if (serverThread.joinable())
			serverThread.join();
	}

	__declspec(dllexport) ModInfo SADXModInfo = { ModLoaderVer };
}
