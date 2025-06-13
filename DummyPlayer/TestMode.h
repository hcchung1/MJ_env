#pragma once
#include "../MJLibrary/MJ_Base/MJBaseTypedef.h"
#include "../MJLibrary/MJ_Base/PlayerTile.h"

class TestMode
{
public:
	TestMode();
	~TestMode();

	void run(const int& argc, char* argv[]);
	void testDummyPlayer(const int& argc, char* argv[]);

	void printState() const;
	void loadIni();

private:
	PlayerTile m_oPlayerTile;
	RemainTile_t m_oRemainTile;
};

