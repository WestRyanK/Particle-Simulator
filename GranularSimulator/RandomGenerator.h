#pragma once
#include <time.h>
#include <random>
#include "../Engine/Dependencies/OpenGL.h"

namespace CodeMonkeys::GranularSimulator
{
	class RandomGenerator
	{
	public:
		static void Init();
		static void Init(unsigned int seed);
		static float RandomBetween(float min, float max);
		static glm::vec3 RandomVecBetween(float min_x, float max_x, float min_y, float max_y, float min_z, float max_z);
		static glm::vec3 RandomVecBetween(float min, float max);
	};
}
