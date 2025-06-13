#include "DummyPlayer.h"
#include "../MJLibrary/Base/ConfigManager.h"
//#include "../MJLibrary/Base/DebugLogger.h"
#ifdef WINDOWS
#include "../CommunicationLibrary/GameBridge.h"
#endif
#include "TestMode.h"
#include <iostream>
using std::cout;
using std::endl;

void init();
void run(int argc, char* argv[]);
void test(int argc, char* argv[]);
void writeConfigInfoToFile();

int main(int argc, char* argv[]) {
	init();
	//test(argc, argv);
	run(argc, argv);
	return 0;
}

void init() {
	//TileGroup::g_bFirstInit = true;

	//ConfigManager::loadConfig();
	// MinLackTable_old::makeTable(MJGameType2::MJGameType_16);
	//MinLackTable::makeTable();
	//WinTable::makeTable();
	//DebugLogger::start("MctsDebugLog");
	//writeConfigInfoToFile();
}

void run(int argc, char* argv[])
{
#ifdef WINDOWS
	DummyPlayer oPlayer;
	GameBridge oGameBridge(&oPlayer);
	bool bUseSocket = ConfigManager::g_bUseSocket;
	if (argc == 3) {
		string sIp = argv[1];
		int iPort = toInt(argv[2]);
		oGameBridge = GameBridge(&oPlayer, sIp, iPort);
	}
	else if (bUseSocket) {
		Ini& oIni = Ini::getInstance();
		string sIp = oIni.getStringIni("Socket.ServerIP");
		int iPort = oIni.getIntIni("Socket.ServerPort");
		oGameBridge = GameBridge(&oPlayer, sIp, iPort);
	}


	while (true) {
		vector<string> vCommand = oGameBridge.getCommand();
		bool bContinue = oGameBridge.handleCommand(vCommand);
		if (!bContinue && !bUseSocket) { break; }
	}
#endif
}

void test(int argc, char* argv[]) {
	TestMode oTest;
	oTest.run(argc, argv);
}

void writeConfigInfoToFile() {
	ofstream fout("MahjongBaseConfigInfo.txt");
	fout << "PLAYER_COUNT=" << PLAYER_COUNT << endl;
	fout << "SUIT_COUNT=" << SUIT_COUNT << endl;
	fout << "MAX_SUIT_TILE_RANK=" << MAX_SUIT_TILE_RANK << endl;
	fout << "HONOR_COUNT=" << HONOR_COUNT << endl;
	fout << "MAX_HONOR_TILE_RANK=" << MAX_HONOR_TILE_RANK << endl;
	fout << "SAME_TILE_COUNT=" << SAME_TILE_COUNT << endl;
	fout << "NEED_GROUP=" << NEED_GROUP << endl;
	fout << "NONPICKABLE_TILE_COUNT=" << NONPICKABLE_TILE_COUNT << endl;
	fout.close();
}
