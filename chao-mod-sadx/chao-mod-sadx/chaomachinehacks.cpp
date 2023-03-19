#include "pch.h"
#include "network.h"
#include "chaomachinehacks.h"

Network* ChaoHacks::networkRef = nullptr;

std::thread ChaoHacks::serverThread;

int ChaoHacks::warningTextMode = 0;

ChaoHacks::ChaoHacks(Network* network)
{
	networkRef = network;
	setupHacks(network);

	warningTextMode = 0;
}

void setupHacks(Network* network)
{
	WriteData<1>((void*)0x74D839, 0); // Limit cursor upwards to 0
	WriteData<1>((void*)0x74D858, 0); // Limit cursor downwards to go back to 0
	WriteData<1>((void*)0x72DC9E, 1); // Initial cursor position
	WriteData<1>((void*)0x72DB7A, 1); // Return cursor position

	WriteCall((void*)0x74D7A6, &CreateAllMainMenuBars);
	WriteData<72>((void*)0x74D7AB, 0x90);
	char AddCorrectESP[] = { 0x81, 0xC4, 0xE8, 0, 0, 0 };
	WriteData(reinterpret_cast<void*>(0x74D7F0), AddCorrectESP, 6);

	// Greyed out drop off fix
	WriteData<float>((float*)0x37C0C60, 128.0f); // width
	WriteData<float>((float*)0x37C0C64, 32.0f); // height
	WriteData<float>((float*)0x37C0C68, 128.1f); // xloc1
	WriteData<float>((float*)0x37C0C6C, 72.1f); // yloc1
	WriteData<float>((float*)0x37C0C70, 254.9f); // xloc2
	WriteData<float>((float*)0x37C0C74, 102.9f); // yloc2

	// Greyed out pick up fix
	WriteData<float>((float*)0x37C0CA8, 128.0f); // width
	WriteData<float>((float*)0x37C0CAC, 32.0f); // height
	WriteData<float>((float*)0x37C0CB0, 128.1f); // xloc1
	WriteData<float>((float*)0x37C0CB4, 103.0f); // yloc1
	WriteData<float>((float*)0x37C0CB8, 254.9f); // xloc2
	WriteData<float>((float*)0x37C0CBC, 134.0f); // yloc2


	// Odekake offset assets
	WriteData<float>((float*)0x74CDF4, 200.0f);
	WriteData<float>((float*)0x74CE0F, 200.0f);
	WriteData<float>((float*)0x74CDB6, 204.0f);
	WriteData<float>((float*)0x74CDC9, 288.0f);
	WriteData<float>((float*)0x74CDDC, 372.0f);

	// Toridasu offset assets
	WriteData<float>((float*)0x74C6D9, 194.0f);
	WriteData<float>((float*)0x74C69C, 200.0f);
	WriteData<float>((float*)0x74C6AF, 284.0f);
	WriteData<float>((float*)0x74C6C2, 368.0f);

	// Write in new functions
	WriteJump((void*)0x719C60, ChaoHacks::startNetworkInMachine);
	WriteCall((void*)0x719A90, ChaoHacks::endNetworkInMachine);
	WriteCall((void*)0x74CF14, ChaoHacks::newOdekakeModes);
	WriteCall((void*)0x74D1B1, ChaoHacks::newOdekakeModes);
	WriteCall((void*)0x74D4DE, ChaoHacks::newOdekakeModes);
	WriteCall((void*)0x74D55A, ChaoHacks::newOdekakeModes);
	char AddCorrectESP2[] = { 0x83, 0xC4, 0x04, 0xC3 }; 
	WriteData(reinterpret_cast<void*>(0x74CF19), AddCorrectESP2, 4);
	WriteData(reinterpret_cast<void*>(0x74D1B6), AddCorrectESP2, 4);
	WriteData(reinterpret_cast<void*>(0x74D4E3), AddCorrectESP2, 4);
	WriteData(reinterpret_cast<void*>(0x74D55F), AddCorrectESP2, 4);

	WriteCall((void*)0x74C86F, ChaoHacks::newToridasuModes);
	WriteData<5>((void*)0x74C888, 0x90);
	WriteData<1>((void*)0x74C88E, 5);
	/*WriteCall((void*)0x74CAB0, ChaoHacks::newToridasuModes);
	WriteData<10>((void*)0x74CABD, 0x90);
	WriteData<char>((char*)0x74CABC, 8);*/
	WriteCall((void*)0x74CAF6, ChaoHacks::newToridasuModes);
	WriteData<54>((void*)0x74CAFB, 0x90);
	WriteData<1>((void*)0x74CB33, 0x18);
	WriteCall((void*)0x74CB55, ChaoHacks::newToridasuModes);
	WriteData<66>((void*)0x74CB5A, 0x90);
	WriteData<1>((void*)0x74CB9E, 8);
	//WriteData<2>((void*)0x74CB9F, 0x90);

	// To make the game think you have a GBA connected do this, it's not very useful on its own tho
	char TrickGameGBAConnected[] = { 0xB8, /*this is the enum for what type of connection it is*/0, 0, 0, 0, 0xC3 };
	WriteData((void*)0x720B06, TrickGameGBAConnected, 6);
	WriteData<1>((void*)0x720AE7, 1); // Write this to have a success state on the gba
}

void CreateAllMainMenuBars()
{
	// Add our own Menu Bars and recreate the ones already in the code
	CreateMainMenuBar(0, 320.0, 148.0, 0x16, dropOffSelectionState);
	CreateMainMenuBar(1, 320.0, 204.0, 0x1E, pickUpSelectionState);
	CreateMainMenuBar(2, 320.0, 260.0, 0x26, namingSelectionState);
	CreateMainMenuBar(3, 320.0, 316.0, 0x2E, goodbyeSelectionState);
	CreateMainMenuBar(4, 320.0, 372.0, 0x36, quitSelectionState);

	// We want to jump back to the original function before the compiled function can do anything else like a return
	__asm 
	{
		mov eax, 0x74D7F0
		jmp eax
	}
}

void ChaoHacks::startNetworkInMachine()
{
	serverThread = std::thread(&Network::runBroadcaster, ChaoHacks::networkRef);
}

void ChaoHacks::endNetworkInMachine()
{
	StopMusic();
	ChaoHacks::networkRef->closeBroadcaster();
	serverThread.join();
}

void AL_OdeMenuSetMode(int mode)
{
	if (pMaster)
	{
		pMaster->mode = mode;
		pMaster->timer = 0;
		pMaster->subtimer = 0;
	}
}

void ChaoHacks::newOdekakeModes()
{
	switch (pMaster->mode)
	{
	case 3:
		AlMsgWarnCreate(0, 80.0f, 96.0f, 480.0f, 144.0f);
		AlMsgWarnOpen(0);
		if (ChaoHacks::networkRef->getIsConnected())
		{
			AlMsgWarnAddLineC(0, (char*)"Warning!");
			AlMsgWarnWaitCont(0);
			AlMsgWarnAddLineC(0, (char*)"This will move the chao currently in the machine to the phone.");
			AlMsgWarnWaitCont(0);
			AlMsgWarnAddLineC(0, (char*)"Are you sure you want to do this?");
			AL_OdeMenuSetMode(6);
		}
		else
		{
			AlMsgWarnAddLineC(0, (char*)"Phone not currently connected.");
			AL_OdeMenuSetMode(20);
		}
		AlMsgWarnWaitClose(0);
		return;
	case 6:
		if (AlMsgWarnGetStatus(0) == 0)
		{
			AlMsgSelectCreate(0, 2, 80.0f, 272.0f, 120.0f);
			AlMsgSelectSetCStr(0, 0, (char*)"Yes");
			AlMsgSelectSetCStr(0, 1, (char*)"No");
			AlMsgSelectSetCursor(0, 1);
			PlaySound(614, 0, 0, 0);
			AL_OdeMenuSetMode(7);
		}
		return;
	case 16: // Yes Selection
		AlMsgWarnClear(0);
		AlMsgWarnAddLineC(0, (char*)"Sending...");
		ChaoHacks::networkRef->sendChao(AL_GBAManagerGetChaoParam());
		AL_OdeMenuSetMode(17);
		return;
	case 17:
		switch (ChaoHacks::networkRef->getIsSentChao())
		{
		case 0:
			return;
		case 1:
			AlMsgWarnAddLineC(0, (char*)"Sent successfully!");
			AlMsgWarnWaitClose(0);
			PlaySound(640, 0, 0, 0);
			AL_ClearChaoParam(AL_GBAManagerGetChaoParam());
			AL_GBAManagerClearChaoParam();
			AL_OdeMenuSetMode(18);
			ChaoHacks::networkRef->confirmChaoMessage();
			return;
		case 2:
			AlMsgWarnAddLineC(0, (char*)"Failure sending!");
			AlMsgWarnWaitClose(0);
		default:
			AL_OdeMenuSetMode(20);
			ChaoHacks::networkRef->confirmChaoMessage();
			return;
		}
	case 13:
		return;
	}
}

void ChaoHacks::newToridasuModes()
{
	switch (pMaster->mode)
	{
	case 3:
		if (ChaoHacks::networkRef->getIsPhoneRequestingSend())
		{
			CHAO_PARAM_GC* newChaoSlot = &AL_GetNewChaoSaveInfo()->param;
			ChaoHacks::networkRef->receiveChao(newChaoSlot);
			AL_OdeMenuSetMode(10);
		}
		return;
	case 5:
		//AlMsgWarnAddLineC(0, (char*)"Your phone isn't already requesting to send its chao.");
		//AlMsgWarnWaitCont(0);
		AlMsgWarnAddLineC(0, (char*)"Waiting for request...");
		AL_OdeMenuSetMode(7);
		return;
	case 7:
		AlMsgSelectCreate(0, 1, 80.0, 280.0, 120.0);
		AlMsgSelectSetCStr(0, 0, (char*)"Cancel");
		AlMsgSelectSetCursor(0, 0);
		return;

	}

}