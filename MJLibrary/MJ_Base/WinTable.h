#pragma once
#include "MahjongBaseConfig.h"
//#include "boost/smart_ptr/make_shared_array.hpp"
#include <memory>

class WinTable
{
public:
	static void makeTable();
	static bool isWin(const int& iMeldNumber, const int vHandTileId[]);

private:
	static int getTileCount(const int& tileRank, const int& tileSetId);
	static int getTripletId(const int& iRank);
	static int getSequenceId(const int& iRank);
	static int getPairId(const int& iRank);

public:
	static int g_iSuitTableSize;
	static int g_iHonorTableSize;
	static std::unique_ptr<int[]> g_vSuitMaxMelds;
	static std::unique_ptr<int[]> g_vSuitMaxMeldsWithPair;
	static std::unique_ptr<int[]> g_vHonorMaxMelds;
	static std::unique_ptr<int[]> g_vHonorMaxMeldsWithPair;
	static std::unique_ptr<int[]> g_vSuitPriority;//meld loss if pick out a pair
	static std::unique_ptr<int[]> g_vHonorPriority;

private:
	//static std::unique_ptr<int[]> powCount;
	static bool m_bFirstInit;
};