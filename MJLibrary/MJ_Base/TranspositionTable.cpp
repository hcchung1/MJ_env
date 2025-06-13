#include "TranspositionTable.h"
#include <random>
#include <functional> // std::bind
#include "../Base/Ini.h"

template <class T>
array<std::unique_ptr<TTEntry_t[]>, SUIT_COUNT> TranspositionTable<T>::g_llZhash_HandTile_Suit;
template <class T>
array<std::unique_ptr<TTEntry_t[]>, HONOR_COUNT> TranspositionTable<T>::g_llZhash_HandTile_Honor;
//template <class T>
//array<std::unique_ptr<uint64_t[]>, SUIT_COUNT> TranspositionTable<T>::g_llZhash_RemainTile_Suit;
//template <class T>
//array<std::unique_ptr<uint64_t[]>, SUIT_COUNT>TranspositionTable<T>::g_llZhash_RemainTile_Honor;
template <class T>
uint64_t TranspositionTable<T>::g_ullSuitTableSize;
template <class T>
uint64_t TranspositionTable<T>::g_ullHonorTableSize;
template <class T>
array<TTEntry_t, MAX_DISCARDED_TILE_COUNT> TranspositionTable<T>::g_llZhash_DrawCount;
template <class T>
T TranspositionTable<T>::m_oEntry[MAX_TT_SIZE] = {};
template <class T>
bool TranspositionTable<T>::m_bEntryUsed[MAX_TT_SIZE] = {};
template <class T>
bool TranspositionTable<T>::m_bFirstInit = true;
template <class T>
bool TranspositionTable<T>::g_bActive = true;
template <class T>
size_t TranspositionTable<T>::m_iUsingEntryCount = 0;
template <class T>
size_t TranspositionTable<T>::m_iCollisionCount = 0;
template <class T>
size_t TranspositionTable<T>::m_iAccessCount = 0;

template<class T>
void TranspositionTable<T>::makeTable()
{
	g_ullSuitTableSize = static_cast<uint64_t>(pow(SAME_TILE_COUNT + 1, MAX_SUIT_TILE_RANK));
	g_ullHonorTableSize = static_cast<uint64_t>(pow(SAME_TILE_COUNT + 1, MAX_HONOR_TILE_RANK));
	if (m_bFirstInit) {
		m_bFirstInit = false;

		Ini& oIni = Ini::getInstance();
		g_bActive = oIni.getIntIni("MCTS.UseTT") > 0;
		if (!g_bActive)
			return;

		for (int i = 0; i < g_llZhash_HandTile_Suit.size(); i++) {
			g_llZhash_HandTile_Suit[i] = std::make_unique<TTEntry_t[]>(g_ullSuitTableSize);
		}
		for (int i = 0; i < g_llZhash_HandTile_Honor.size(); i++) {
			g_llZhash_HandTile_Honor[i] = std::make_unique<TTEntry_t[]>(g_ullHonorTableSize);
		}
		
		//typedef boost::uniform_int<uint64_t> NumberDistribution;
		//typedef boost::mt19937 RandomNumberGenerator;
		//typedef boost::variate_generator<RandomNumberGenerator&, NumberDistribution> Generator;
		//NumberDistribution distribution(0, (std::numeric_limits<uint64_t>::max)());
		//RandomNumberGenerator generator;
		//Generator numberGenerator(generator, distribution);

		std::random_device rd;
		//std::default_random_engine gen = std::default_random_engine(rd());
		std::mt19937 gen(rd());
		std::uniform_int_distribution<TTEntry_t> dis(0, TTEntry_Max);
		auto numberGenerator = std::bind(dis, gen);

		for (int i = 0; i < g_llZhash_HandTile_Suit.size(); i++) {
			for (int j = 0; j < g_ullSuitTableSize; j++) {
				g_llZhash_HandTile_Suit[i][j] = numberGenerator();
			}
		}

		for (int i = 0; i < g_llZhash_HandTile_Honor.size(); i++) {
			for (int j = 0; j < g_ullHonorTableSize; j++) {
				g_llZhash_HandTile_Honor[i][j] = numberGenerator();
			}
		}

		for (int i = 0; i < MAX_DISCARDED_TILE_COUNT; i++) {
			g_llZhash_DrawCount[i] = numberGenerator();
		}

		for (int i = 0; i < MAX_TT_SIZE; i++) {
			m_bEntryUsed[i] = false;
		}
	}
}

template<class T>
void TranspositionTable<T>::clearTable() {
	if (!g_bActive)
		return;

	for (int i = 0; i < MAX_TT_SIZE; i++) {
		m_bEntryUsed[i] = false;
	}
	m_iAccessCount = 0;
	m_iCollisionCount = 0;
	m_iUsingEntryCount = 0;
}

template<class T>
void TranspositionTable<T>::insertBoardData(const unsigned long long& llHashKey, const T& oData) {
	if (!g_bActive)
		return;

	const unsigned long long ullIndex = llHashKey & BIT_TT_MASK;
	//[Analysis]
	m_iAccessCount++;
	if (m_bEntryUsed[ullIndex]) {
		//already used
		m_iCollisionCount++;
	}
	else {
		m_iUsingEntryCount++;
	}
	//[Analysis] end

	m_oEntry[ullIndex] = oData;
	m_bEntryUsed[ullIndex] = true;
};
