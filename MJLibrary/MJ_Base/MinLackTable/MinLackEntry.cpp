#include "MinLackEntry.h"
#include <sstream>
#include <iostream>
using std::stringstream;
using std::cerr;
using std::endl;

MinLackEntry::MinLackEntry()
	: m_iTileNumber(0)
	, m_iMeld(0)
	, m_iPair(0)
	, m_iSequ(0)
	, m_iUsefulBits(0)
	, m_iLargerOne(0)
	, m_iAlone(0)
	, m_iSevenPairUsefulBits(0)
	, m_iGoshimusoPart(0)
	, m_bGoshimusoPair(false)
	, m_iGoshimusoUsefulBits(0)
	, m_bCostPair(false)
	, m_bSecondChoice(false)
	, m_eSecond(nullptr)
{
	//m_vKnittedSeqTilesCount_suit.fill(0);
	//m_vKnittedSeqTiles_suit.fill(bitset<3>(0));
}

MinLackEntry::~MinLackEntry()
{
	
}

void MinLackEntry::setSecondChoice(const MinLackEntry &entry)
{
	m_bSecondChoice = true;
	//m_eSecond = std::make_shared<MinLackEntry>();
	m_eSecond = new MinLackEntry();
	*m_eSecond = entry;
}

void MinLackEntry::inputEntry(ifstream& inputFd)
{
	inputFd.read((char*)this, sizeof(*this));
	if (m_bSecondChoice) {
		//std::cerr << "second choice" << std::endl;
		//m_eSecond = std::make_shared<MinLackEntry>();//this will crash. Why?
		m_eSecond = new MinLackEntry();
		m_eSecond->inputEntry(inputFd);
	}
}

void MinLackEntry::outputEntry(ofstream& outputFd)
{
	outputFd.write((char*)this, sizeof(*this));
	if (m_bSecondChoice)
		m_eSecond->outputEntry(outputFd);
};

string MinLackEntry::toString()
{
	stringstream result;
	result
		<< m_iTileNumber << " "
		<< m_iMeld << " "
		<< m_iPair << " "
		<< m_iSequ << " "
		<< m_iUsefulBits << " "
		<< m_iLargerOne << " "
		<< m_iAlone << " "
		<< m_iSevenPairUsefulBits << " "
		<< m_iGoshimusoPart << " "
		<< m_bGoshimusoPair << " "
		<< m_iGoshimusoUsefulBits << " "
		<< m_bCostPair << " ";
	//for (int i = 0; i < m_vKnittedSeqTilesCount_suit.size(); i++) {
	//	result << m_vKnittedSeqTilesCount_suit[i] << " ";
	//}
	for (int i = 0; i < m_vKnittedSeqTiles_suit.size(); i++){
		result << m_vKnittedSeqTiles_suit[i].to_string() << " ";
	}
	result << m_bSecondChoice;
	if (m_bSecondChoice)
		result << std::endl << m_eSecond->toString();

	return result.str();
}