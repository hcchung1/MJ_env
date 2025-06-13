#include "TestMode.h"
#include "DummyPlayer.h"
#include "../MJLibrary/Base/Ini.h"

TestMode::TestMode()
{
}


TestMode::~TestMode()
{
}

void TestMode::run(const int& argc, char* argv[])
{
	loadIni();
	testDummyPlayer(argc, argv);
}


void TestMode::testDummyPlayer(const int & argc, char * argv[])
{
	m_oPlayerTile = PlayerTile("34a 124667b 123c 33A ( 111A )");
	m_oRemainTile = RemainTile_t("4322232322240213123334424140222423", RemainTileType::Type_Playing);
	printState();
	std::cout << "[TestMode::testDummyPlayer] MinLack: " << m_oPlayerTile.getMinLack() << std::endl;

	DummyPlayer oPlayer(m_oPlayerTile, m_oRemainTile);
	Tile oDrawnTile("6c");
	oPlayer.drawTile(oDrawnTile);
	std::cout << "[TestMode::testDummyPlayer] Player draws " << oDrawnTile.toString() << std::endl;

	Tile oThrownTile = oPlayer.askThrow();
	std::cout << "[TestMode::testDummyPlayer] Ans: " << Move(MoveType::Move_Throw, oThrownTile).toString() << std::endl;

	std::cout << std::endl << "end" << std::endl;
}

void TestMode::printState() const
{
	std::cerr << "PlayerTile: " << m_oPlayerTile.toString() << std::endl;
	std::cerr << "RemainTile:" << std::endl << m_oRemainTile.getReadableString() << std::endl;
}

void TestMode::loadIni()
{
	std::cerr << "[TestMode::loadIni] Start." << std::endl;
	Ini& oIni = Ini::getInstance();
	std::cout << oIni.getIntIni("MCTS.TotalSampleCount") << std::endl;
}
