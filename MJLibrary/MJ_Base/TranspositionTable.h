#pragma once
#include <iostream>
#include <unordered_map>
#include <memory>
#include "MinLackTable/MinLackTable.h"
#include "MahjongBaseConfig.h"
//#include "..\Sampling_Base\SamplingData.h"

#define MAX_TT_SIZE 33554432 //2^25
//#define MAX_TT_SIZE 1048576 //2^20
//#define MAX_TT_SIZE 65536 //2^16
#define BIT_TT_MASK (MAX_TT_SIZE - 1)
//#define SUIT_BOOL_SIZE pow(2, MAX_SUIT_RANK)//2^9, for RemainTile(=0 or >0)
//#define HONOR_BOOL_SIZE pow(2, MAX_HONOR_RANK)//2^7, for RemainTile(=0 or >0)

typedef uint64_t TTEntry_t;
#define TTEntry_Max UINT64_MAX
using std::array;

template <class T>
class TranspositionTable {
public:
	TranspositionTable() {};
	~TranspositionTable() {};

	static array<std::unique_ptr<TTEntry_t[]>, SUIT_COUNT> g_llZhash_HandTile_Suit;
	static array<std::unique_ptr<TTEntry_t[]>, HONOR_COUNT> g_llZhash_HandTile_Honor;
	static uint64_t g_ullSuitTableSize;
	static uint64_t g_ullHonorTableSize;
	//static array<std::unique_ptr<uint64_t[]>, SUIT_COUNT> g_llZhash_RemainTile_Suit;
	//static array<std::unique_ptr<uint64_t[]>, HONOR_COUNT> g_llZhash_RemainTile_Honor;
	static array<TTEntry_t, MAX_DISCARDED_TILE_COUNT> g_llZhash_DrawCount;
	static bool g_bActive;

	static void makeTable();
	static void clearTable();

	static T getBoardData(const long long& llHashKey) {
		if (!g_bActive)
			return T();

		const unsigned long long ullIndex = llHashKey & BIT_TT_MASK;
		if (!isDataExist(ullIndex)) {
			return T();
		}

		return m_oEntry[ullIndex];
	};
	static void insertBoardData(const unsigned long long& llHashKey, const T& oData);
	static bool isDataExist(const unsigned long long& llHashKey) {
		if (!g_bActive)
			return false;
		const unsigned long long ullIndex = llHashKey & BIT_TT_MASK;
		return m_bEntryUsed[ullIndex];
	};

	//[Analysis]
	static double getEntryUsingRate() { return g_bActive? static_cast<double>(m_iUsingEntryCount) / MAX_TT_SIZE : -1.0; };
	static double getCollisionRate() { return g_bActive? static_cast<double>(m_iCollisionCount) / m_iAccessCount : -1.0; };

private:
	static bool m_bEntryUsed[MAX_TT_SIZE];
	static T m_oEntry[MAX_TT_SIZE];
	static bool m_bFirstInit;

	//analysis
	static size_t m_iUsingEntryCount;
	static size_t m_iCollisionCount;
	static size_t m_iAccessCount;
};


