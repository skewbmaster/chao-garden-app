#pragma once

struct ODE_MENU_MASTER_WORK
{
	int PreStage;
	int CurrStage;
	int NextStage;
	int mode;
	int timer;
	int subtimer;
	int counter;
	int state;
	int EndFlag;
	int cursorX;
	int cursorY;
	ObjectMaster* tp;
	void* mpStageWork;
	void(__cdecl* mfStageExit)(ODE_MENU_MASTER_WORK*);
};

FunctionPointer(void, CreateMainMenuBar, (char id, float xpos, float ypos, unsigned short waittime, char state), 0x76D3D0);
FunctionPointer(int, AL_OdeMenuGetMode, (), 0x72DBA0);
FunctionPointer(void, AlMsgWarnCreate, (int winid, float px, float py, float sx, float sy), 0x72D770);
FunctionPointer(void, AlMsgWarnOpen, (int winid), 0x72D880);
FunctionPointer(void, AlMsgWarnWaitCont, (int winid), 0x72D910);
FunctionPointer(void, AlMsgWarnAddLineC, (int winid, char* message), 0x72D8D0);
FunctionPointer(void, AlMsgWarnWaitClose, (int winid), 0x72D930);
FunctionPointer(bool, AlMsgWarnGetStatus, (int winid), 0x72D860);
FunctionPointer(void, AlMsgWarnClear, (int winid), 0x72D8B0);
FunctionPointer(void, AlMsgWarnDelete, (int winid), 0x72D820);

FunctionPointer(void, CreateSelectorMenu, (int winid, float px, float py, float sx, char* optionText1, char* optionText2), 0x74AD40);
FunctionPointer(void, AlMsgSelectCreate, (int winid, int itemnum, float px, float py, float sx), 0x72D2F0);
FunctionPointer(void, AlMsgSelectSetCStr, (int winid, int itemid, char* text), 0x72D390);
FunctionPointer(void, AlMsgSelectSetCursor, (int winid, int cursor), 0x72D370);
FunctionPointer(bool, AlMsgSelectIsActive, (int winid), 0x72D3D0);
FunctionPointer(int, AlMsgSelectGetCursor, (int winid), 0x72D3E0);

FunctionPointer(chao_save_info*, AL_GetNewChaoSaveInfo, (), 0x715E60); // Gets next available chao slot
FunctionPointer(CHAO_PARAM_GC*, AL_GBAManagerGetChaoParam, (), 0x716B70);
FunctionPointer(void, AL_ClearChaoParam, (CHAO_PARAM_GC*), 0x730B20);
FunctionPointer(void, AL_GBAManagerClearChaoParam, (), 0x716B40);
DataPointer(ODE_MENU_MASTER_WORK*, pMaster, 0x3CDCA24);

DataPointer(char, dropOffSelectionState, 0x3CE0560);
DataPointer(char, pickUpSelectionState, 0x3CE0564);
DataPointer(char, namingSelectionState, 0x3CE0568);
DataPointer(char, goodbyeSelectionState, 0x3CE056C);
DataPointer(char, quitSelectionState, 0x3CE0570);



void setupHacks(Network* network);
void newOdekake();
void CreateAllMainMenuBars();

class ChaoHacks
{
public:
	ChaoHacks(Network* network);
	static Network* networkRef;
	static std::thread serverThread;

	static int warningTextMode;

	static void startNetworkInMachine();
	static void endNetworkInMachine();
	static void newOdekakeModes();
	static void newToridasuModes();
};

