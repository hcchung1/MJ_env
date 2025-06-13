#include "MinLackTable_old.h"
//#include <iostream>
#include <string>
#include <algorithm>
#include <fstream>
#include <chrono>
#include <cassert>
#ifdef LINUX
#include "Tools.h"
#else
#include "..\..\Base\Tools.h"
#endif

using std::string;
using std::ifstream;
using std::ofstream;

//MinLackEntry MinLackTable_old::g_eSuit[SUIT_TABLE_SIZE];
//MinLackEntry MinLackTable_old::g_eHonor[HONOR_TABLE_SIZE];
//TileGroup_t MinLackTable_old::allTile[9] = { 1, 5, 25, 125, 625, 3125, 15625, 78125, 390625 };
//TileGroup_t MinLackTable_old::allMeld[16] = { 31, 155, 775, 3875, 19375, 96875, 484375, 3, 15, 75, 375, 1875, 9375, 46875, 234375, 1171875 };
//TileGroup_t MinLackTable_old::allPair[9] = { 2, 10, 50, 250, 1250, 6250, 31250, 156250, 781250 };
//TileGroup_t MinLackTable_old::allSequ[15] = { 6, 30, 150, 750, 3750, 18750, 93750, 468750, 26, 130, 650, 3250, 16250, 81250, 406250 };

array<MinLackEntry, SUIT_TABLE_SIZE> MinLackTable_old::g_eSuit;
array<MinLackEntry, HONOR_TABLE_SIZE> MinLackTable_old::g_eHonor;
array<TileGroup_t, 9> MinLackTable_old::allTile = { 1, 5, 25, 125, 625, 3125, 15625, 78125, 390625 };
array<TileGroup_t, 16> MinLackTable_old::allMeld = { 31, 155, 775, 3875, 19375, 96875, 484375, 3, 15, 75, 375, 1875, 9375, 46875, 234375, 1171875 };
array<TileGroup_t, 9> MinLackTable_old::allPair = { 2, 10, 50, 250, 1250, 6250, 31250, 156250, 781250 };
array<TileGroup_t, 15> MinLackTable_old::allSequ = { 6, 30, 150, 750, 3750, 18750, 93750, 468750, 26, 130, 650, 3250, 16250, 81250, 406250 };

#ifdef CONST_MINLACKTABLE_GAMETYPE
const MJGameType2 MinLackTable_old::g_mjGameType = MJGameType2::MJGameType_CSM;
#else
MJGameType2 MinLackTable_old::g_mjGameType = MJGameType2::MJGameType_16;
#endif
bool MinLackTable_old::g_bFirstSetup = true;


string MinLackTable_old::m_sTablePath;
int MinLackTable_old::iMaxHandTileNumber;

void MinLackTable_old::makeTable(const MJGameType2& mjGameType)
{
	//assert(mjGameType == MJGameType2::MJGameType_16 || mjGameType == MJGameType2::MJGameType_Jpn);
	if (!g_bFirstSetup)
		return;

	g_bFirstSetup = false;
	// Have some bug that g_mjGameType will be illegally modified in _Server.cpp
	// We currently set g_mjGameType to a constant variable.
#ifndef CONST_MINLACKTABLE_GAMETYPE
	g_mjGameType = mjGameType; 
#endif
	if (g_mjGameType == MJGameType2::MJGameType_16) {
		m_sTablePath = getCurrentPath() + "MinLackTable_old(Sampling)";
		iMaxHandTileNumber = 17;
	}
	else if (g_mjGameType == MJGameType2::MJGameType_Jpn) {
		m_sTablePath = getCurrentPath() + "MinLackTable_old(Jpn)";
		iMaxHandTileNumber = 14;
	}
	else if (g_mjGameType == MJGameType2::MJGameType_CSM) {
		m_sTablePath = getCurrentPath() + "MinLackTable_old(CSM)";
		iMaxHandTileNumber = 14;
	}
	
	TileGroup::g_bFirstInit = true;

	ifstream existFd(m_sTablePath);
	if (existFd.is_open()) {
		existFd.close();
		CERR("[MinLackTable_old::makeTable] " + m_sTablePath + " was found. Load from the built file.\n");
		loadTable();
		//assert(g_mjGameType == MJGameType2::MJGameType_16 || g_mjGameType == MJGameType2::MJGameType_Jpn);
		return;
	} else
		existFd.close();

	CERR("[MinLackTable_old::makeTable] " + m_sTablePath + " was not found. Start making MinLackTable.\n");
	//TileGroup_t remainTiles;
	//std::cout << "[MinLackTable_old::makeTable] start setHonorEntry." << std::endl;
	auto startTime = std::chrono::steady_clock::now();
	for (TileGroup_t honorTiles = 0; honorTiles < HONOR_TABLE_SIZE; ++honorTiles) {
		if (honorTiles.getTileNumber() > iMaxHandTileNumber)
			continue;
		setHonorEntry(honorTiles, g_eHonor[honorTiles]);
		//if((int)honorTiles % 1000 == 0){
			//std::cerr << "[MinLackTable_old::makeTable] honorTiles=" << (int)honorTiles << " done." << std::endl;
		//}
	}
	
	//std::cout << "[MinLackTable_old::makeTable] start setSuitEntry." << std::endl;
	for (TileGroup_t suitTiles = 0; suitTiles < SUIT_TABLE_SIZE; ++suitTiles) {
		if (suitTiles.getTileNumber() > iMaxHandTileNumber)
			continue;
		setSuitEntry(suitTiles, g_eSuit[suitTiles]);
		//if((int)suitTiles % 1000 == 0){
			//std::cerr << "[MinLackTable_old::makeTable] suitTiles=" << (int)suitTiles << " done." << std::endl;
		//}
	}

	auto usedTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - startTime);
	CERR("[MinLackTable_old::makeTable] The table is built in " + std::to_string(usedTime.count()) + "  ms.\n");

	saveTable();
	//assert(g_mjGameType == MJGameType2::MJGameType_16 || g_mjGameType == MJGameType2::MJGameType_Jpn);
}

void MinLackTable_old::saveTable()
{
	ofstream outputFd;

	CERR("[MinLackTable_old::saveTable] Start saving table to " + m_sTablePath + " .\n");
	auto startTime = std::chrono::steady_clock::now();

	outputFd.open(m_sTablePath, std::ios::out | std::ios::binary);
	for (TileGroup_t honorTiles = 0; honorTiles < HONOR_TABLE_SIZE; ++honorTiles) {
		if (honorTiles.getTileNumber() <= iMaxHandTileNumber)
			g_eHonor[honorTiles].outputEntry(outputFd);
	}
	for (TileGroup_t suitTiles = 0; suitTiles < SUIT_TABLE_SIZE; ++suitTiles) {
		if (suitTiles.getTileNumber() <= iMaxHandTileNumber)
			g_eSuit[suitTiles].outputEntry(outputFd);
	}
	outputFd.close();
	
	auto usedTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - startTime);
	CERR("[MinLackTable_old::saveTable] The table was saved to " + m_sTablePath + " in " + std::to_string(usedTime.count()) + " ms.\n");
}

void MinLackTable_old::loadTable()
{
	ifstream inputFd;
	//uint32_t iAvailableSuitEntries = 0, iAvailableHonorEntries = 0;

	auto startTime = std::chrono::steady_clock::now();
	inputFd.open(m_sTablePath, std::ios::in | std::ios::binary);
	for (TileGroup_t honorTiles = 0; honorTiles < HONOR_TABLE_SIZE; ++honorTiles) {
		if (honorTiles.getTileNumber() <= iMaxHandTileNumber) {
			g_eHonor[honorTiles].inputEntry(inputFd);
			//iAvailableHonorEntries++;
		}
	}
	for (TileGroup_t suitTiles = 0; suitTiles < SUIT_TABLE_SIZE; ++suitTiles) {
		if (suitTiles.getTileNumber() <= iMaxHandTileNumber) {
			g_eSuit[suitTiles].inputEntry(inputFd);
			//iAvailableSuitEntries++;
		}
	}
	inputFd.close();
	auto usedTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - startTime);
	std::cerr << "[MinLackTable_old::loadTable] The table is loaded in " << usedTime.count() << " ms."  << std::endl;
	//assert(g_mjGameType == MJGameType2::MJGameType_16 || g_mjGameType == MJGameType2::MJGameType_Jpn);
	//std::cerr << "Available entries: [Suit] " << iAvailableSuitEntries << ", [Honor] " << iAvailableHonorEntries << std::endl;
}

int MinLackTable_old::pickOutMeld(TileGroup_t &tiles, const int &startPickOutIndex)
{
	TileGroup_t sourceTiles = tiles;
	TileGroup_t remainTiles;
	TileGroup_t _remainTiles;
	int maxMeld = 0;
	int bestPair = 0;
	int bestSequ = 0;
	int bestLack = 0;
	int meldNumber;
	int pairNumber;
	int sequNumber;
	int lackNumber;

	for (int i = startPickOutIndex; i < 16; i++) {
		if (sourceTiles.canPickOut(allMeld[i])) {
			remainTiles = sourceTiles;
			remainTiles.pickOut(allMeld[i]);
			//why not g_eSuit[remainTiles].m_iMeld + 1?
			//remainTiles will change after pickOutMeld(remainTiles, i);
			meldNumber = pickOutMeld(remainTiles, i) + 1;
			_remainTiles = remainTiles;
			
			pairNumber = g_eSuit[remainTiles].m_iPair;
			sequNumber = g_eSuit[remainTiles].m_iSequ;
			lackNumber = pairNumber + sequNumber;//tatsu number?

			if (maxMeld < meldNumber || (maxMeld == meldNumber && bestLack < lackNumber) || (maxMeld == meldNumber && bestLack == lackNumber && bestPair < pairNumber)) {
				maxMeld = meldNumber;
				bestPair = pairNumber;
				bestSequ = sequNumber;
				bestLack = lackNumber;
				tiles = _remainTiles;
			}
		}
	}

	//assert(g_mjGameType == MJGameType2::MJGameType_16 || g_mjGameType == MJGameType2::MJGameType_Jpn);
	return maxMeld;
}

int MinLackTable_old::pickOutPair(TileGroup_t &tiles, const int &startPickOutIndex)
{
	//return the best pair's count that maximize pairNumber + sequNumber
	TileGroup_t sourceTiles = tiles;
	TileGroup_t remainTiles = tiles;
	TileGroup_t _remainTiles;
	int bestPair = 0;
	int bestSequ = pickOutSequ(remainTiles, 0);//why??
	int bestLack = bestSequ;//why?? It seems like TatsuNumber (lackNumber = pairNumber + sequNumber;)
	int pairNumber;
	int sequNumber;
	int lackNumber;

	for (int i = startPickOutIndex; i < 9; i++) {
		if (sourceTiles.canPickOut(allPair[i])) {
			remainTiles = sourceTiles;
			remainTiles.pickOut(allPair[i]);
			pairNumber = pickOutPair(remainTiles, i) + 1;//why not g_eSuit[remainTiles].m_iPair + 1?

			_remainTiles = remainTiles;

			sequNumber = g_eSuit[remainTiles].m_iSequ;
			lackNumber = pairNumber + sequNumber;

			if (bestLack < lackNumber || (bestLack == lackNumber && bestPair < pairNumber)) {
				bestPair = pairNumber;
				bestSequ = sequNumber;
				bestLack = lackNumber;
				tiles = _remainTiles;
			}
		}
	}

	//assert(g_mjGameType == MJGameType2::MJGameType_16 || g_mjGameType == MJGameType2::MJGameType_Jpn);
	return bestPair;
}

int MinLackTable_old::pickOutSequ(TileGroup_t &tiles, const int &startPickOutIndex)
{
	//return the subseuqnce's count 
	//(No matter some of them can form to pair. This case will be handle in getNormalMinLackNumber() by calculating hasCostPair(tiles, ...))
	TileGroup_t sourceTiles = tiles;
	TileGroup_t remainTiles;
	int maxSequNumber = 0;
	int sequNumber;

	for (int i = startPickOutIndex; i < 15; i++) {
		if (sourceTiles.canPickOut(allSequ[i])) {
			remainTiles = sourceTiles;
			remainTiles.pickOut(allSequ[i]);
			sequNumber = pickOutSequ(remainTiles, i) + 1;//why not g_eSuit[remainTiles].m_iSequ + 1?

			if (maxSequNumber < sequNumber) {
				maxSequNumber = sequNumber;
				tiles = remainTiles;
			}
		}
	}

	//assert(g_mjGameType == MJGameType2::MJGameType_16 || g_mjGameType == MJGameType2::MJGameType_Jpn);
	return maxSequNumber;
}

int MinLackTable_old::pickOutAllPair(TileGroup_t &tiles)
{
	int result = 0;
	for (int i = 0; i < 9; i++) {
		while(tiles.canPickOut(allPair[i])) {
			tiles.pickOut(allPair[i]);
			result += 1;
		}
	}

	//assert(g_mjGameType == MJGameType2::MJGameType_16 || g_mjGameType == MJGameType2::MJGameType_Jpn);
	return result;
}

bool MinLackTable_old::hasCostPair(TileGroup_t tiles, const int &meldNumber, const int &pairNumber, const int &sequNumber, const int &startPickOutIndex)
{
	if (meldNumber == 0) {
		bool condition1 = pickOutAllPair(tiles) > pairNumber;//pick out all pairs whatever.
		bool condition2 = pickOutSequ(tiles, 0) < sequNumber;//after all pairs were picked out, the incompleteSequence will be not ideal.
		return condition1 && condition2;
	}
	
	TileGroup_t remainTiles = tiles;
	for (int i = startPickOutIndex; i < 16; i++) {
		if (tiles.canPickOut(allMeld[i])) {
			remainTiles = tiles;
			remainTiles.pickOut(allMeld[i]);
			if (hasCostPair(remainTiles, meldNumber - 1, pairNumber, sequNumber, i))
				return true;
		}
	}

	//assert(g_mjGameType == MJGameType2::MJGameType_16 || g_mjGameType == MJGameType2::MJGameType_Jpn);
	return false;
}

MinLackEntry MinLackTable_old::findSecondChoice(TileGroup_t &tiles)
{
	MinLackEntry result;
	TileGroup_t remainTiles;
	int meldNumber;
	int pairNumber;
	int sequNumber;
	int lackNumber;

	for (int i = 0; i < 9; i++) {
		if (tiles.canPickOut(allPair[i])) {
			remainTiles = tiles;
			remainTiles.pickOut(allPair[i]);

			meldNumber = g_eSuit[remainTiles].m_iMeld;
			pairNumber = g_eSuit[remainTiles].m_iPair + 1;
			sequNumber = g_eSuit[remainTiles].m_iSequ;
			lackNumber = pairNumber + sequNumber;

			if (g_eSuit[tiles].m_iMeld - 1 <= meldNumber && g_eSuit[tiles].m_iPair + g_eSuit[tiles].m_iSequ + 3 <= lackNumber) {
				result.m_iMeld = meldNumber;
				result.m_iPair = pairNumber;
				result.m_iSequ = sequNumber;
				return result;
			}
		}
	}

	for (int i = 0; i < 15; i++) {
		if (tiles.canPickOut(allSequ[i])) {
			remainTiles = tiles;
			remainTiles.pickOut(allSequ[i]);

			meldNumber = g_eSuit[remainTiles].m_iMeld;
			pairNumber = g_eSuit[remainTiles].m_iPair;
			sequNumber = g_eSuit[remainTiles].m_iSequ + 1;
			lackNumber = pairNumber + sequNumber;

			if (g_eSuit[tiles].m_iMeld - 1 <= meldNumber && g_eSuit[tiles].m_iPair + g_eSuit[tiles].m_iSequ + 3 <= lackNumber) {
				result.m_iMeld = meldNumber;
				result.m_iPair = pairNumber;
				result.m_iSequ = sequNumber;
				return result;
			}
		}
	}

	//assert(g_mjGameType == MJGameType2::MJGameType_16 || g_mjGameType == MJGameType2::MJGameType_Jpn);
	result.m_iMeld = -1;
	return result;
}

int MinLackTable_old::getMinLackNumber(const int *pHandeTileID)
{
	return getMinLackNumber(pHandeTileID, getNeedMeldNumber(pHandeTileID));
}

int MinLackTable_old::getMinLackNumber(const int *pHandeTileID, int iNeedGroup, int iNeedEye)
{
	//return getNormalMinLackNumber(pHandeTileID, iNeedGroup, iNeedEye);

	int minLack;
	int tileNumber;

	switch (g_mjGameType) {
	case MJGameType2::MJGameType_16:
		return getNormalMinLackNumber(pHandeTileID, iNeedGroup, iNeedEye);
	case MJGameType2::MJGameType_Jpn:
		minLack = getNormalMinLackNumber(pHandeTileID, iNeedGroup, iNeedEye);
		tileNumber = getHandTileNumber(pHandeTileID);

		if (tileNumber == 13 || tileNumber == 14) {
			int sevenPairMinLack = getSevenPairMinLackNumber(pHandeTileID);
			int goshimusoMinLack = getGoshimusoMinLackNumber(pHandeTileID);

			if (sevenPairMinLack < minLack)
				minLack = sevenPairMinLack;
			if (goshimusoMinLack < minLack)
				minLack = goshimusoMinLack;
		}
		return minLack;
	case MJGameType2::MJGameType_CSM:
		minLack = getNormalMinLackNumber(pHandeTileID, iNeedGroup, iNeedEye);
		tileNumber = getHandTileNumber(pHandeTileID);

		if (tileNumber == 13 || tileNumber == 14) {
			int sevenPairMinLack = getSevenPairMinLackNumber(pHandeTileID);
			int goshimusoMinLack = getGoshimusoMinLackNumber(pHandeTileID);
			int honorsAndKnittedTilesMinLack = getHonorsAndKnittedTilesMinLackNumber(pHandeTileID);
			int knittedStraightMinLack = getKnittedStraightMinLackNumber(pHandeTileID);

			if (sevenPairMinLack < minLack)
				minLack = sevenPairMinLack;
			if (goshimusoMinLack < minLack)
				minLack = goshimusoMinLack;
			if (honorsAndKnittedTilesMinLack < minLack)
				minLack = honorsAndKnittedTilesMinLack;
			if (knittedStraightMinLack < minLack)
				minLack = knittedStraightMinLack;
		}
		return minLack;
	default:
		std::cerr << "[MinLackTable_old::getMinLackNumber] Illegal g_mjGameType: " << static_cast<int>(g_mjGameType) << std::endl;
		//assert(g_mjGameType == MJGameType2::MJGameType_16 || g_mjGameType == MJGameType2::MJGameType_Jpn);
		return -1;
	}
}

int MinLackTable_old::getNormalMinLackNumber(const int *pHandeTileID, int iNeedGroup, int iNeedEye)
{
	MinLackEntry *tiles[3];
	int minLack;
	int totalMeldNumber = iNeedGroup;
	int needMeldNumber;
	int hasMeldNumber = g_eHonor[pHandeTileID[3]].m_iMeld;
	int pairNumber = g_eHonor[pHandeTileID[3]].m_iPair;
	int incompleteSequNumber = 0;
	int lackTileToPairNumber;
	int lackOneNumber;
	int lackTwoNumber;

	for (int i = 0; i < 3; i++) {
		tiles[i] = &g_eSuit[pHandeTileID[i]];
		hasMeldNumber += tiles[i]->m_iMeld;
		pairNumber += tiles[i]->m_iPair;
		incompleteSequNumber += tiles[i]->m_iSequ;
	}
	needMeldNumber = totalMeldNumber - hasMeldNumber;

	//Case 1: 1235689a235689b2355c
	//needMeldNumber = 5 - 1 = 4
	//incompleteSequNumber = 2 + 3 + 1 = 6
	//Result: incompleteSequNumber - 2 == needMeldNumber, but no cost pair
	/*std::cerr << "[MinLackTable_old::getNormalMinLackNumber] incompleteSequNumber - 2 =  " << incompleteSequNumber - 2
		<< ", needMeldNumber = " << needMeldNumber << std::endl;*/
	if (incompleteSequNumber - 2 >= needMeldNumber) {//why??
		for (int i = 0; i < 3; i++) {
			if (tiles[i]->hasCostPair()) {
				/*std::cerr << "[MinLackTable_old::getNormalMinLackNumber] update  pairNumber from " << pairNumber
					<< " to " << pairNumber + 1 << ", incompleteSequNumber from " << incompleteSequNumber
					<< " to " << incompleteSequNumber - 2 << std::endl;*/
				pairNumber += 1;
				incompleteSequNumber -= 2;
				break;
			}
		}
	}

	for (int i = 0; needMeldNumber > incompleteSequNumber + pairNumber && i < 3; i++) {
		if (tiles[i]->hasSecondChoice()) {
			hasMeldNumber += tiles[i]->m_eSecond->m_iMeld - tiles[i]->m_iMeld;
			pairNumber += tiles[i]->m_eSecond->m_iPair - tiles[i]->m_iPair;
			incompleteSequNumber += tiles[i]->m_eSecond->m_iSequ - tiles[i]->m_iSequ;
			needMeldNumber = totalMeldNumber - hasMeldNumber;
		}
	}

	/*std::cerr << "[MinLackTable_old::getNormalMinLackNumber] hasMeldNumber = " << hasMeldNumber
		<< ", pairNumber = " << pairNumber
		<< ", incompleteSequNumber = " << incompleteSequNumber << std::endl;*/

	lackTileToPairNumber = pairNumber > 0 ? 0 : 1;

	lackOneNumber = std::max(0, std::min(needMeldNumber, incompleteSequNumber + std::max(0, pairNumber - 1)));
	lackTwoNumber = std::max(0, needMeldNumber - lackOneNumber);

	minLack = lackOneNumber + 2 * lackTwoNumber + lackTileToPairNumber;

	/*std::cerr << "[MinLackTable_old::getNormalMinLackNumber] lackOneNumber = " << lackOneNumber
		<< ", lackTwoNumber = " << lackTwoNumber
		<< ", lackTileToPairNumber = " << lackTileToPairNumber << std::endl;*/

	//assert(g_mjGameType == MJGameType2::MJGameType_16 || g_mjGameType == MJGameType2::MJGameType_Jpn);
	return minLack;
}

int MinLackTable_old::getSevenPairMinLackNumber(const int* pHandeTileID)
{
	int sevenPairMinLack;
	int sevenPairLackPairNumber = 7 - g_eHonor[pHandeTileID[3]].m_iLargerOne;
	int aloneNumber = g_eHonor[pHandeTileID[3]].m_iAlone;

	for (int i = 0; i < 3; i++) {
		sevenPairLackPairNumber -= g_eSuit[pHandeTileID[i]].m_iLargerOne;
		aloneNumber += g_eSuit[pHandeTileID[i]].m_iAlone;
	}

	sevenPairMinLack = aloneNumber < sevenPairLackPairNumber ? 2 * sevenPairLackPairNumber - aloneNumber : sevenPairLackPairNumber;
	//assert(g_mjGameType == MJGameType2::MJGameType_16 || g_mjGameType == MJGameType2::MJGameType_Jpn);
	return sevenPairMinLack;
}

int MinLackTable_old::getGoshimusoMinLackNumber(const int* pHandeTileID)
{
	int goshimusoMinLack;
	int goshimusoPart = g_eHonor[pHandeTileID[3]].m_iGoshimusoPart;
	bool goshimusoPair = g_eHonor[pHandeTileID[3]].m_bGoshimusoPair;

	for (int i = 0; i < 3; i++) {
		goshimusoPart += g_eSuit[pHandeTileID[i]].m_iGoshimusoPart;
		goshimusoPair |= g_eSuit[pHandeTileID[i]].m_bGoshimusoPair;
	}

	goshimusoMinLack = 13 - goshimusoPart;
	if (!goshimusoPair)
		goshimusoMinLack += 1;

	//assert(g_mjGameType == MJGameType2::MJGameType_16 || g_mjGameType == MJGameType2::MJGameType_Jpn);
	return goshimusoMinLack;
}

int MinLackTable_old::getHonorsAndKnittedTilesMinLackNumber(const int* pHandeTileID)
{
	const array<array<size_t, 3>, 6> vIndexs = { {
		{0, 1, 2}
		, {0, 2, 1}
		, {1, 0, 2}
		, {1, 2, 0}
		, {2, 0, 1}
		, {2, 1, 0}
	} };

	//print g_eSuit[pHandeTileID[i]].m_vKnittedSeqTilesCount_suit[i] (for i = 0, 1, 2 and i = 0, 1, 2)
	//std::cerr << "[MinLackTable_old::getHonorsAndKnittedTilesMinLackNumber] Input:" << std::endl;
	//for (int i = 0; i < 3; i++) {
	//	std::cerr << pHandeTileID[i];
	//	for (int i = 0; i < 3; i++)
	//		std::cerr << " " << g_eSuit[pHandeTileID[i]].m_vKnittedSeqTilesCount_suit[i];
	//	std::cerr << std::endl;
	//}

	// find max sum of pHandeTileID[vIndex][i] (for i = 0, 1, 2 and vIndex in vIndexs)
	int iKnittedTileSum = 0, iSum;
	for (auto vIndex : vIndexs) {
		iSum = 0;
		for (int j = 0; j < vIndex.size(); j++) {
			//iSum += g_eSuit[pHandeTileID[vIndex[j]]].m_vKnittedSeqTilesCount_suit[j];
			iSum += g_eSuit[pHandeTileID[vIndex[j]]].m_vKnittedSeqTiles_suit[j].count();
		}
		iKnittedTileSum = std::max(iKnittedTileSum, iSum);
	}

	assert(g_eHonor[pHandeTileID[3]].m_iGoshimusoPart - iKnittedTileSum <= 14);
	return 14 - g_eHonor[pHandeTileID[3]].m_iGoshimusoPart - iKnittedTileSum;
}

int MinLackTable_old::getKnittedStraightMinLackNumber(const int* pHandeTileID)
{
	const array<array<size_t, 3>, 6> vIndexs = { {
		{0, 1, 2}
		, {0, 2, 1}
		, {1, 0, 2}
		, {1, 2, 0}
		, {2, 0, 1}
		, {2, 1, 0}
	} };
	int iSuit;

	//for i = 0~ vIndexs.size() - 1, if g_eSuit[pHandeTileID[vIndex[i][i]]].m_vKnittedSeqTilesCount_suit[i] == 3 for all i
	//then pick out this three knitted straights (147, 258, 369) from the three suits
	int iBestMinLack = MAX_MINLACK, iMinLack, iNormalMinLack;
	array<int, TILE_TYPE_COUNT> vCopiedHandTileId;
	int iKnittedTileCount;
	for (auto vIndex : vIndexs) {
		std::copy(pHandeTileID, pHandeTileID + TILE_TYPE_COUNT, vCopiedHandTileId.begin());
		iKnittedTileCount = 0;
		for (int i = 0; i < vIndex.size(); i++) {
			iSuit = vIndex[i];
			for (int j = 0; j < 3; j++) {
				if (g_eSuit[vCopiedHandTileId[iSuit]].m_vKnittedSeqTiles_suit[i].test(j)) {
					vCopiedHandTileId[iSuit] -= allTile[i + j * 3];
					iKnittedTileCount++;
				}
			}
		}
		/*
		std::cerr << "[MinLackTable_old::getKnittedStraightMinLackNumber]" << std::endl;
		for (int i = 0; i < SUIT_COUNT; i++) {
			//std::cerr << "vCopiedHandTileId[" << i << "] = " << vCopiedHandTileId[i] << std::endl;
			std::cerr << "TileGroup_t(vCopiedHandTileId[" << i << "]) = " << TileGroup_t(vCopiedHandTileId[i]).toString() << std::endl;
		}
		//std::cerr << "vCopiedHandTileId[" << SUIT_COUNT << "] = " << vCopiedHandTileId[SUIT_COUNT] << std::endl;
		std::cerr << "TileGroup_t(vCopiedHandTileId[" << SUIT_COUNT << "]) = " << TileGroup_t(vCopiedHandTileId[SUIT_COUNT]).toString() << std::endl;
		std::cerr << "iKnittedTileCount = " << iKnittedTileCount << std::endl;
		//std::cerr << "Need group = " << iMaxHandTileNumber / 3 - 3 << std::endl;
		*/
		
		iNormalMinLack = getNormalMinLackNumber(vCopiedHandTileId.data(), iMaxHandTileNumber / 3 - 3, 1);
		assert(iNormalMinLack >= 0 && iNormalMinLack <= 9);
		/*
		std::cerr << "iNormalMinLack = " << iNormalMinLack
			<< " " << (iNormalMinLack >= 0)
			<< " " << (iNormalMinLack <= 9)
			<< std::endl;
		*/

		iMinLack = 9 - iKnittedTileCount + iNormalMinLack;
		//std::cerr << "MinLack = " << iMinLack << std::endl << std::endl;
		iBestMinLack = std::min(iBestMinLack, iMinLack);
	}

	return iBestMinLack;
}

int MinLackTable_old::getHandTileNumber(const int *pHandeTileID)
{
	int tileNumber = g_eHonor[pHandeTileID[3]].m_iTileNumber;
	for (int i = 0; i < 3; i++)
		tileNumber += g_eSuit[pHandeTileID[i]].m_iTileNumber;

	//assert(g_mjGameType == MJGameType2::MJGameType_16 || g_mjGameType == MJGameType2::MJGameType_Jpn);
	return tileNumber;
}

int MinLackTable_old::getNeedMeldNumber(const int *pHandeTileID)
{
	//assert(g_mjGameType == MJGameType2::MJGameType_16 || g_mjGameType == MJGameType2::MJGameType_Jpn);
	return getHandTileNumber(pHandeTileID) / 3;
}

int MinLackTable_old::getMeldNumber(const int *pHandeTileID)
{
	MinLackEntry *tiles[3];
	int totalMeldNumber = getNeedMeldNumber(pHandeTileID);
	int needMeldNumber;
	int hasMeldNumber = g_eHonor[pHandeTileID[3]].m_iMeld;
	int pairNumber = g_eHonor[pHandeTileID[3]].m_iPair;
	int incompleteSequNumber = 0;

	for (int i = 0; i < 3; i++) {
		tiles[i] = &g_eSuit[pHandeTileID[i]];
		hasMeldNumber += tiles[i]->m_iMeld;
		pairNumber += tiles[i]->m_iPair;
		incompleteSequNumber += tiles[i]->m_iSequ;

	}
	needMeldNumber = totalMeldNumber - hasMeldNumber;

	if (incompleteSequNumber - 2 >= needMeldNumber) {
		for (int i = 0; i < 3; i++) {
			if (tiles[i]->hasCostPair()) {
				pairNumber += 1;
				incompleteSequNumber -= 2;
				break;
			}
		}
	}

	for (int i = 0; needMeldNumber > incompleteSequNumber + pairNumber && i < 3; i++) {
		if (tiles[i]->hasSecondChoice()) {
			hasMeldNumber += tiles[i]->m_eSecond->m_iMeld - tiles[i]->m_iMeld;
			pairNumber += tiles[i]->m_eSecond->m_iPair - tiles[i]->m_iPair;
			incompleteSequNumber += tiles[i]->m_eSecond->m_iSequ - tiles[i]->m_iSequ;
			needMeldNumber = totalMeldNumber - hasMeldNumber;
		}
	}

	//assert(g_mjGameType == MJGameType2::MJGameType_16 || g_mjGameType == MJGameType2::MJGameType_Jpn);
	return hasMeldNumber;
}

int MinLackTable_old::getPairAndIncompleteSequNumber(const int *pHandeTileID)
{
	MinLackEntry *tiles[3];
	int totalMeldNumber = getNeedMeldNumber(pHandeTileID);
	int needMeldNumber;
	int hasMeldNumber = g_eHonor[pHandeTileID[3]].m_iMeld;
	int pairNumber = g_eHonor[pHandeTileID[3]].m_iPair;
	int incompleteSequNumber = 0;

	for (int i = 0; i < 3; i++) {
		tiles[i] = &g_eSuit[pHandeTileID[i]];
		hasMeldNumber += tiles[i]->m_iMeld;
		pairNumber += tiles[i]->m_iPair;
		incompleteSequNumber += tiles[i]->m_iSequ;
	}
	needMeldNumber = totalMeldNumber - hasMeldNumber;

	if (incompleteSequNumber - 2 >= needMeldNumber) {
		for (int i = 0; i < 3; i++) {
			if (tiles[i]->hasCostPair()) {
				pairNumber += 1;
				incompleteSequNumber -= 2;
				break;
			}
		}
	}

	for (int i = 0; needMeldNumber > incompleteSequNumber + pairNumber && i < 3; i++) {
		if (tiles[i]->hasSecondChoice()) {
			hasMeldNumber += tiles[i]->m_eSecond->m_iMeld - tiles[i]->m_iMeld;
			pairNumber += tiles[i]->m_eSecond->m_iPair - tiles[i]->m_iPair;
			incompleteSequNumber += tiles[i]->m_eSecond->m_iSequ - tiles[i]->m_iSequ;
			needMeldNumber = totalMeldNumber - hasMeldNumber;
		}
	}

	//assert(g_mjGameType == MJGameType2::MJGameType_16 || g_mjGameType == MJGameType2::MJGameType_Jpn);
	return pairNumber + incompleteSequNumber;
}

bool MinLackTable_old::hasLeftTile(const TileGroup_t & tiles, const int &index)
{
	//assert(g_mjGameType == MJGameType2::MJGameType_16 || g_mjGameType == MJGameType2::MJGameType_Jpn);
	return index >= 0 && index < 9 && tiles[index] < '4';
}

void MinLackTable_old::setSevenPair(TileGroup_t &tiles, MinLackEntry &entry)
{
	if (g_mjGameType == MJGameType2::MJGameType_Jpn || g_mjGameType == MJGameType2::MJGameType_CSM) {
		for (int i = 0; i < 9; i++) {
			if (tiles[i] >= '2')
				entry.m_iLargerOne += 1;
			else if (tiles[i] == '1')
				entry.m_iAlone += 1;
		}
	}
	//assert(g_mjGameType == MJGameType2::MJGameType_16 || g_mjGameType == MJGameType2::MJGameType_Jpn);
}

void MinLackTable_old::setGoshimuso(TileGroup_t& tiles, MinLackEntry& entry)
{
	//[TODO]
}

void MinLackTable_old::setKnittedSequ(TileGroup_t& tiles, MinLackEntry& entry)
{
	if (g_mjGameType == MJGameType2::MJGameType_CSM) {
		//init entry.m_vKnittedSeqTilesCount_suit (necessary?)
		//entry.m_vKnittedSeqTilesCount_suit.fill(0);
		entry.m_vKnittedSeqTiles_suit.fill(bitset<3>(0));
		//for (int i = 0; i < entry.m_vKnittedSeqTiles_suit.size(); i++) {
		//	std::cerr << entry.m_vKnittedSeqTiles_suit[i].to_string() << " ";
		//}
		//std::cerr << std::endl;

		//for tile $t$ from 0 to 9, if there is at least one $t$, increase entry.m_vKnittedSeqTilesCount_suit[$t$ % 3] by 1.
		for (int i = 0; i < 9; i++) {
			if (tiles[i] > '0') {
				//entry.m_vKnittedSeqTilesCount_suit[i % 3]++;
				entry.m_vKnittedSeqTiles_suit[i % 3][i / 3] = 1;
			}
		}
	}
}

void MinLackTable_old::setSuitUsefulBits(TileGroup_t &tiles, MinLackEntry &entry)
{
	for (int i = 0; i < 9; i++) {
		if (tiles[i] > '0') {
			if (hasLeftTile(tiles, i))
				entry.m_iUsefulBits |= (0x01 << i);
			if (hasLeftTile(tiles, i - 2))
				entry.m_iUsefulBits |= (0x01 << (i - 2));
			if (hasLeftTile(tiles, i - 1))
				entry.m_iUsefulBits |= (0x01 << (i - 1));
			if (hasLeftTile(tiles, i + 1))
				entry.m_iUsefulBits |= (0x01 << (i + 1));
			if (hasLeftTile(tiles, i + 2))
				entry.m_iUsefulBits |= (0x01 << (i + 2));
		}
		if (g_mjGameType == MJGameType2::MJGameType_Jpn) {
			if (tiles[i] == '1')
				entry.m_iSevenPairUsefulBits |= (0x01 << i);
		}
	}
	//assert(g_mjGameType == MJGameType2::MJGameType_16 || g_mjGameType == MJGameType2::MJGameType_Jpn);
}

void MinLackTable_old::setHonorEntry(TileGroup_t &tiles, MinLackEntry &entry)
{
	for (int i = 0; i < 7; i++) {
		if (tiles[i] >= '3')
			entry.m_iMeld += 1;
		else if (tiles[i] == '2')
			entry.m_iPair += 1;
	}

	for (int i = 0; i < 7; i++) {
		if (tiles[i] >= '1' && tiles[i] < '4')
			entry.m_iUsefulBits |= (0x01 << i);

		if (g_mjGameType == MJGameType2::MJGameType_Jpn || g_mjGameType == MJGameType2::MJGameType_CSM) {
			if (tiles[i] == '0' || (entry.m_iPair == 0 && entry.m_iMeld == 0))
				entry.m_iGoshimusoUsefulBits |= (0x01 << i);
			if (tiles[i] == '1')
				entry.m_iSevenPairUsefulBits |= (0x01 << i);
			if (tiles[i] > '0')
				entry.m_iGoshimusoPart += 1;
			if (tiles[i] >= '2')
				entry.m_bGoshimusoPair = true;
		}
	}
	entry.m_iTileNumber = tiles.getTileNumber();
	setSevenPair(tiles, entry);
	//assert(g_mjGameType == MJGameType2::MJGameType_16 || g_mjGameType == MJGameType2::MJGameType_Jpn);
}

void MinLackTable_old::setSuitEntry(TileGroup_t &tiles, MinLackEntry &entry)
{
	setSuitUsefulBits(tiles, entry);

	TileGroup_t remainTiles = tiles;
	entry.m_iMeld = pickOutMeld(remainTiles, 0);

	if (entry.m_iMeld > 0) {
		entry.m_iPair = g_eSuit[remainTiles].m_iPair;
		entry.m_iSequ = g_eSuit[remainTiles].m_iSequ;
	} else {
		entry.m_iPair = pickOutPair(remainTiles, 0);
		entry.m_iSequ = pickOutSequ(remainTiles, 0);
	}
	entry.m_bCostPair = hasCostPair(tiles, entry.m_iMeld, entry.m_iPair, entry.m_iSequ, 0);

	entry.m_iTileNumber = tiles.getTileNumber();
	setSevenPair(tiles, entry);
	setKnittedSequ(tiles, entry);

	if (tiles.hasPair()) {
		MinLackEntry secondEntry = findSecondChoice(tiles);
		if (secondEntry.m_iMeld >= 0) {
			secondEntry.m_bCostPair = hasCostPair(tiles, secondEntry.m_iMeld, secondEntry.m_iPair, secondEntry.m_iSequ, 0);
			secondEntry.m_iLargerOne = entry.m_iLargerOne;
			secondEntry.m_iAlone = entry.m_iAlone;
			if (g_mjGameType == MJGameType2::MJGameType_Jpn || g_mjGameType == MJGameType2::MJGameType_CSM) {
				secondEntry.m_iSevenPairUsefulBits = entry.m_iSevenPairUsefulBits;
				secondEntry.m_iGoshimusoPart = entry.m_iGoshimusoPart;
				secondEntry.m_bGoshimusoPair = entry.m_bGoshimusoPair;
			}
			entry.setSecondChoice(secondEntry);
		}
	}

	/*
	if (g_mjGameType == MJGameType2::MJGameType_Jpn) {
		if (tiles[0] > '0')
			entry.m_iGoshimusoPart += 1;
		if (tiles[8] > '0')
			entry.m_iGoshimusoPart += 1;
		if (tiles[0] >= '2' || tiles[8] >= '2')
			entry.m_bGoshimusoPair = true;
		if (tiles[0] == '0' || (tiles[0] == '1' && tiles[8] < '2'))
			entry.m_iGoshimusoUsefulBits |= (0x01 << 0);
		if (tiles[8] == '0' || (tiles[8] == '1' && tiles[0] < '2'))
			entry.m_iGoshimusoUsefulBits |= (0x01 << 8);
	}
	*/
	//assert(g_mjGameType == MJGameType2::MJGameType_16 || g_mjGameType == MJGameType2::MJGameType_Jpn);
}