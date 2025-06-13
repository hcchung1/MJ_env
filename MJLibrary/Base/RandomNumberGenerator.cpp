#include "RandomNumberGenerator.h"

void RandomNumberGenerator::setSeed(const int & iSeed)
{
	m_iSeed = iSeed;
	if (iSeed > 0)
		gen.seed(iSeed);
}

int RandomNumberGenerator::random_Int()
{
	return dis_int(gen);
}

double RandomNumberGenerator::random_Double()
{
	return dis_double(gen);
}

