#ifndef __MIN_LACK_TABLE__
#define __MIN_LACK_TABLE__

#define SUIT_TABLE_SIZE 1953125
#define HONOR_TABLE_SIZE 78125
//#define CONST_MINLACKTABLE_GAMETYPE

#include <array>
#include "MinLackEntry.h"
#include "TileGroup.h"
using std::array;
typedef TileGroup TileGroup_t;

enum class MJGameType2 { MJGameType_16, MJGameType_Jpn, MJGameType_CSM };

class MinLackTable_old
{
public:
	static void makeTable(const MJGameType2& mjGameType);
	static int getMinLackNumber(const int *pHandeTileID);
	static int getMinLackNumber(const int *pHandeTileID, int iNeedGroup, int iNeedEye = 1);
	static int getNormalMinLackNumber(const int *pHandeTileID, int iNeedGroup, int iNeedEye);
	static int getSevenPairMinLackNumber(const int* pHandeTileID);
	static int getGoshimusoMinLackNumber(const int *pHandeTileID);
	static int getHonorsAndKnittedTilesMinLackNumber(const int* pHandeTileID);
	static int getKnittedStraightMinLackNumber(const int* pHandeTileID);
	static int getHandTileNumber(const int *pHandeTileID);
	static int getNeedMeldNumber(const int *pHandeTileID);
	static int getMeldNumber(const int *pHandeTileID);
	static int getPairAndIncompleteSequNumber(const int *pHandeTileID);

	static bool isFirstSetup() { return g_bFirstSetup; }
	static MJGameType2 getMjGameType() { return g_mjGameType; }

private:
	static int pickOutMeld(TileGroup_t &tiles, const int &startPickOutIndex);
	static int pickOutPair(TileGroup_t &tiles, const int &startPickOutIndex);
	static int pickOutSequ(TileGroup_t &tiles, const int &startPickOutIndex);
	static int pickOutAllPair(TileGroup_t &tiles);
	static bool hasCostPair(TileGroup_t tiles, const int &meldNumber, const int &pairNumber, const int &sequNumber, const int &startPickOutIndex);
	static bool hasLeftTile(const TileGroup_t & tiles, const int &index);
	static MinLackEntry findSecondChoice(TileGroup_t &tiles);
	static void setSevenPair(TileGroup_t &tiles, MinLackEntry &entry);
	static void setGoshimuso(TileGroup_t& tiles, MinLackEntry& entry);
	static void setKnittedSequ(TileGroup_t& tiles, MinLackEntry& entry);
	static void setSuitUsefulBits(TileGroup_t &tiles, MinLackEntry &entry);
	static void setHonorEntry(TileGroup_t &tiles, MinLackEntry &entry);
	static void setSuitEntry(TileGroup_t &tiles, MinLackEntry &entry);
	static void saveTable();
	static void loadTable();

public:
	//for gtest 
	friend class MinLackTableTest_setKnittedSequTest_Test;
	friend class MinLackTableTest_setKnittedSequTest1_Test;


	//static MinLackEntry g_eSuit[SUIT_TABLE_SIZE];
	//static MinLackEntry g_eHonor[HONOR_TABLE_SIZE];
	//static TileGroup_t allTile[9];
	//static TileGroup_t allMeld[16];
	//static TileGroup_t allPair[9];
	//static TileGroup_t allSequ[15];
	static array<MinLackEntry, SUIT_TABLE_SIZE> g_eSuit;
	static array<MinLackEntry, HONOR_TABLE_SIZE> g_eHonor;
	static array<TileGroup_t, 9> allTile;
	static array<TileGroup_t, 16> allMeld;
	static array<TileGroup_t, 9> allPair;
	static array<TileGroup_t, 15> allSequ;

	static string m_sTablePath;
	static int iMaxHandTileNumber;

private:
#ifdef CONST_MINLACKTABLE_GAMETYPE
	static const MJGameType2 g_mjGameType;
#else
	static MJGameType2 g_mjGameType;
#endif
	
	static bool g_bFirstSetup;
};
#endif