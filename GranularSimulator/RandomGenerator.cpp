#include "RandomGenerator.h"

using CodeMonkeys::GranularSimulator::RandomGenerator;

void RandomGenerator::Init(unsigned int seed)
{
	std::srand(seed);
}

void RandomGenerator::Init()
{
	std::srand((unsigned int)time(NULL));
}

float RandomGenerator::RandomBetween(float min, float max)
{
	float range = max - min;
	float fractional = (float)std::rand() / RAND_MAX;

	float randomValue = min + fractional * range;
	return randomValue;
}

glm::vec3 RandomGenerator::RandomVecBetween(float min_x, float max_x, float min_y, float max_y, float min_z, float max_z)
{
	return glm::vec3(
		RandomGenerator::RandomBetween(min_x, max_x),
		RandomGenerator::RandomBetween(min_y, max_y),
		RandomGenerator::RandomBetween(min_z, max_z));
}

glm::vec3 RandomGenerator::RandomVecBetween(float min, float max)
{
	return RandomGenerator::RandomVecBetween(min, max, min, max, min, max);
}
