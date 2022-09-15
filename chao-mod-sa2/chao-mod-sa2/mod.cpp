#include "pch.h"
#include "network.h"

HelperFunctions HelperFunctionsGlobal;
std::string modpath;

std::thread serverThread;

extern "C" {
	__declspec(dllexport) void __cdecl Init(const char* path, const HelperFunctions& helperFunctions)
	{
		HelperFunctionsGlobal = helperFunctions; // Save the helper pointer for external use
		modpath = path;

		setupServer();
	}

	__declspec(dllexport) void __cdecl OnFrame()
	{
		
	}

	__declspec(dllexport) void __cdecl OnInput()
	{
		PDS_PERIPHERAL* controller = ControllerPointers[0];

		if (controller)
		{
			if (controller->press & Buttons_Up)
			{
				if (serverThread.joinable())
					serverThread.join();
				
				serverThread = std::thread(runServer);
			}
			else if (controller->press & Buttons_Left)
			{
				serverThread.join();
			}
		}
	}

	__declspec(dllexport) void __cdecl OnExit()
	{
		cleanupNetwork();

		if (serverThread.joinable())
			serverThread.join();
	}

	__declspec(dllexport) ModInfo SA2ModInfo = { ModLoaderVer };
}