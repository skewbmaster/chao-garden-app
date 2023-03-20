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
	}

	__declspec(dllexport) void __cdecl OnFrame()
	{
		// Change the output of the get GBA connection kind function regardless of what's
		// happening so that the GBA is shown if the phone is connected
		if (network.getIsConnected())
		{
			WriteData<1>((void*)0x720B07, 3);
		}
		else
		{
			WriteData<1>((void*)0x720B07, 0);
		}
	}

	__declspec(dllexport) void __cdecl OnExit()
	{
		// Thankfully this is called when the game is quit so 
		printf("exiting\n");
		network.cleanupNetwork();

		if (chaohacks.serverThread.joinable())
			chaohacks.serverThread.join();
	}

	__declspec(dllexport) ModInfo SADXModInfo = { ModLoaderVer };
}
