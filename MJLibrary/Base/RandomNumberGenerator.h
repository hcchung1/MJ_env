#pragma once
#include <random>
class RandomNumberGenerator
{
public:
	RandomNumberGenerator(const int& iSeed = -1) : m_iSeed(iSeed) { setSeed(iSeed); };
	~RandomNumberGenerator() {};
	
	void setSeed(const int& iSeed = -1);
	inline int getSeed() const { return m_iSeed; };
	int random_Int();
	double random_Double();

	void operator=(const RandomNumberGenerator& rhs) { setSeed(rhs.getSeed()); };

private:
	int m_iSeed;
	std::random_device rd;
	std::mt19937 gen;
	std::uniform_real_distribution<double> dis_double;
	std::uniform_int_distribution<int> dis_int;
};

