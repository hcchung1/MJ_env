#include "WinTable.h"
#include <algorithm>
#include <fstream>
#include <cmath>
#include <climits>
#include "MinLackTable/TileGroup.h"
#include "../Base/Tools.h"
#define max(a,b) ((a)>(b))? (a) : (b)

using std::cerr;
using std::endl;

std::unique_ptr<int[]> WinTable::g_vSuitMaxMelds;
std::unique_ptr<int[]> WinTable::g_vSuitMaxMeldsWithPair;
std::unique_ptr<int[]> WinTable::g_vHonorMaxMelds;
std::unique_ptr<int[]> WinTable::g_vHonorMaxMeldsWithPair;
std::unique_ptr<int[]> WinTable::g_vSuitPriority;
std::unique_ptr<int[]> WinTable::g_vHonorPriority;
int WinTable::g_iSuitTableSize;
int WinTable::g_iHonorTableSize;
bool WinTable::m_bFirstInit = true;

void WinTable::makeTable()
{
	//int pair[10] = { 0, 2, 10, 50, 250, 1250, 6250, 31250, 156250, 781250 };
	//int triplet[10] = { 0, 3, 15, 75, 375, 1875, 9375, 46875, 234375, 1171875 };
	//int Sequence[8] = { 0, 31, 155, 775, 3875, 19375, 96875, 484375 };

	if (m_bFirstInit) {
		m_bFirstInit = false;
		g_iSuitTableSize = pow(SAME_TILE_COUNT + 1, MAX_SUIT_TILE_RANK);
		g_iHonorTableSize = pow(SAME_TILE_COUNT + 1, MAX_HONOR_TILE_RANK);

		g_vSuitMaxMelds = std::make_unique<int[]>(g_iSuitTableSize);
		g_vSuitMaxMeldsWithPair = std::make_unique<int[]>(g_iSuitTableSize);
		g_vHonorMaxMelds = std::make_unique<int[]>(g_iHonorTableSize);
		g_vHonorMaxMeldsWithPair = std::make_unique<int[]>(g_iHonorTableSize);
		g_vSuitPriority = std::make_unique<int[]>(g_iSuitTableSize);
		g_vHonorPriority = std::make_unique<int[]>(g_iHonorTableSize);

		//Suit Table
		for (int i = 0; i < g_iSuitTableSize; i++) {
			g_vSuitMaxMelds[i] = 0;
			g_vSuitMaxMeldsWithPair[i] = -1000;
		}

		for (int i = 0; i < g_iSuitTableSize; i++) {
			//add triplet
			for (int j = 1; j <= MAX_SUIT_TILE_RANK; j++) {
				if (/*i + vTriplet[j] < g_iSuitTableSize &&*/ getTileCount(j, i) <= SAME_TILE_COUNT - 3)
					g_vSuitMaxMelds[i + getTripletId(j)] = max(g_vSuitMaxMelds[i + getTripletId(j)], g_vSuitMaxMelds[i] + 1);
			}
			//add sequence
			for (int j = 1; j <= MAX_SUIT_TILE_RANK - 2; j++) {
				if (/*i + vSequence[j] < g_iSuitTableSize
					&&*/ getTileCount(j, i) <= SAME_TILE_COUNT - 1
					&& getTileCount(j + 1, i) <= SAME_TILE_COUNT - 1
					&& getTileCount(j + 2, i) <= SAME_TILE_COUNT - 1)
					g_vSuitMaxMelds[i + getSequenceId(j)] = max(g_vSuitMaxMelds[i + getSequenceId(j)], g_vSuitMaxMelds[i] + 1);
			}
		}

		for (int i = 0; i < g_iSuitTableSize; i++) {
			//add pair
			for (int j = 1; j <= MAX_SUIT_TILE_RANK; j++) {
				if (/*i + vPair[j] < g_iSuitTableSize && */getTileCount(j, i) <= SAME_TILE_COUNT - 2)
					g_vSuitMaxMeldsWithPair[i + getPairId(j)] = max(g_vSuitMaxMeldsWithPair[i + getPairId(j)], g_vSuitMaxMelds[i]);
			}
		}

		//compute meld loss if pick out a pair
		for (int i = 0; i < g_iSuitTableSize; i++)
			g_vSuitPriority[i] = g_vSuitMaxMelds[i] - g_vSuitMaxMeldsWithPair[i];

		//Honor Table
		for (int i = 0; i < g_iHonorTableSize; i++) {
			g_vHonorMaxMelds[i] = 0;
			g_vHonorMaxMeldsWithPair[i] = -1000;
		}

		for (int i = 0; i < g_iHonorTableSize; i++) {
			//add triplet
			for (int j = 1; j <= MAX_HONOR_TILE_RANK; j++) {
				if (getTileCount(j, i) <= SAME_TILE_COUNT - 3)
					g_vHonorMaxMelds[i + getTripletId(j)] = max(g_vHonorMaxMelds[i + getTripletId(j)], g_vHonorMaxMelds[i] + 1);
			}
		}

		for (int i = 0; i < g_iHonorTableSize; i++) {
			//add pair
			for (int j = 1; j <= MAX_HONOR_TILE_RANK; j++) {
				if (getTileCount(j, i) <= SAME_TILE_COUNT - 2)
					g_vHonorMaxMeldsWithPair[i + getPairId(j)] = max(g_vHonorMaxMeldsWithPair[i + getPairId(j)], g_vHonorMaxMelds[i]);
			}
		}

		//compute meld loss if pick out a pair
		for (int i = 0; i < g_iHonorTableSize; i++)
			g_vHonorPriority[i] = g_vHonorMaxMelds[i] - g_vHonorMaxMeldsWithPair[i];
	}

	CERR("[WinTable::makeTable] finished making table.\n");
}

bool WinTable::isWin(const int& iMeldNumber, const int vHandTileId[])
{
	bool bInvalid = false;
	for (int i = 0; i < TILE_TYPE_COUNT; i++) {
		int iMaxRank = i < SUIT_COUNT ? MAX_SUIT_TILE_RANK : MAX_HONOR_TILE_RANK;
		if (vHandTileId[i] < 0 || vHandTileId[i] >= pow(SAME_TILE_COUNT + 1, iMaxRank)) {
			bInvalid = true;
		}
	}
	if (bInvalid) {
		std::cerr << "vHandTileId=";
		for (int i = 0; i < TILE_TYPE_COUNT; i++) {
			std::cerr << vHandTileId[i] << " ";
		}
		std::cerr << std::endl;
		std::cerr << "Illegal Id!" << std::endl;
	}

	int meldSum = 0;
	for (int i = 0; i < SUIT_COUNT; i++)
		meldSum += g_vSuitMaxMelds[vHandTileId[i]];
	for (int i = 0; i < HONOR_COUNT; i++)
		meldSum += g_vHonorMaxMelds[vHandTileId[SUIT_COUNT + i]];

	if (meldSum < iMeldNumber)
		return false;

	int minP = INT_MAX;
	for (int i = 0; i < SUIT_COUNT; i++) {
		if (minP > g_vSuitPriority[vHandTileId[i]])
			minP = g_vSuitPriority[vHandTileId[i]];
	}
	for (int i = 0; i < HONOR_COUNT; i++) {
		if (minP > g_vHonorPriority[vHandTileId[SUIT_COUNT + i]])
			minP = g_vHonorPriority[vHandTileId[SUIT_COUNT + i]];
	}

	/*if (meldSum - minP >= iMeldNumber) {
		std::cerr << meldSum << " " << minP << " " << iMeldNumber << std::endl;
		std::cerr << "isWin" << std::endl;
	}*/
	return meldSum - minP >= iMeldNumber;
}

int WinTable::getTileCount(const int& tileRank, const int& tileSetId)
{
	//int powCount[10] = { 1, 5, 25, 125, 625, 3125, 15625, 78125, 390625, 1953125 };
	if (tileRank == MAX_SUIT_TILE_RANK) {
		return tileSetId / TileGroup::g_allTile[tileRank - 1];
	}
	return (tileSetId % TileGroup::g_allTile[tileRank]) / TileGroup::g_allTile[tileRank - 1];
}

int WinTable::getTripletId(const int& iRank)
{
	return TileGroup::g_allMeld[iRank - 1 + MAX_SUIT_TILE_RANK - 2];
}

int WinTable::getSequenceId(const int& iRank)
{
	return TileGroup::g_allMeld[iRank - 1];
}

int WinTable::getPairId(const int& iRank)
{
	return TileGroup::g_allPair[iRank - 1];
}
