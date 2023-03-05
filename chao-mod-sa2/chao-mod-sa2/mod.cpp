#include "pch.h"
#include "network.h"

HelperFunctions HelperFunctionsGlobal;
std::string modpath;

std::thread serverThread;

Network network;

extern "C" {
	__declspec(dllexport) void __cdecl Init(const char* path, const HelperFunctions& helperFunctions)
	{
		HelperFunctionsGlobal = helperFunctions; // Save the helper pointer for external use
		modpath = path;

		network = Network();
		network.setupServer();
	}

	__declspec(dllexport) void __cdecl OnFrame()
	{
		
	}

	__declspec(dllexport) void __cdecl OnInput()
	{
		PDS_PERIPHERAL* controller = ControllerPointers[0];

		if (controller)
		{
			if (controller->press & Buttons_Y && controller->on & Buttons_Left)
			{
				if (serverThread.joinable())
					serverThread.join();

				printf("started broadcaster\n");
				
				serverThread = std::thread(&Network::runBroadcaster, &network);
			}
			/*else if (controller->press & Buttons_Left)
			{
				serverThread.join();
			}*/
		}
	}

	__declspec(dllexport) void __cdecl OnExit()
	{
		printf("exiting\n");
		network.cleanupNetwork();

		if (serverThread.joinable())
			serverThread.join();
	}

	__declspec(dllexport) ModInfo SA2ModInfo = { ModLoaderVer };
}