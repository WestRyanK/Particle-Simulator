#include "BodyOffsets.h"

using CodeMonkeys::GranularSimulator::BodyOffsets;

std::vector<glm::vec3>* BodyOffsets::tetrahedron_grain;
std::vector<glm::vec3>* BodyOffsets::cube_grain;
std::vector<glm::vec3>* BodyOffsets::sphere_grain;


std::vector<glm::vec3>* BodyOffsets::get_tetrahedron_grain(float particle_size)
{
	if (BodyOffsets::tetrahedron_grain == nullptr)
	{
		BodyOffsets::tetrahedron_grain = new std::vector<glm::vec3>();
		BodyOffsets::tetrahedron_grain->push_back(glm::vec3(1.0f, 0.0f, -1.0f / glm::sqrt(2.0f)) * particle_size);
		BodyOffsets::tetrahedron_grain->push_back(glm::vec3(-1.0f, 0.0f, -1.0f / glm::sqrt(2.0f)) * particle_size);
		BodyOffsets::tetrahedron_grain->push_back(glm::vec3(0.0f, 1.0f, 1.0f / glm::sqrt(2.0f)) * particle_size);
		BodyOffsets::tetrahedron_grain->push_back(glm::vec3(0.0f, -1.0f, 1.0f / glm::sqrt(2.0f)) * particle_size);
	}

	return BodyOffsets::tetrahedron_grain;
}
std::vector<glm::vec3>* BodyOffsets::get_cube_grain(float particle_size)
{
	if (BodyOffsets::cube_grain == nullptr)
	{
		BodyOffsets::cube_grain = new std::vector<glm::vec3>();
		BodyOffsets::cube_grain->push_back(glm::vec3(0.0f, 0.0f, 0.0f) * particle_size);
		BodyOffsets::cube_grain->push_back(glm::vec3(1.0f, 1.0f, 1.0f) * particle_size);
		BodyOffsets::cube_grain->push_back(glm::vec3(1.0f, 1.0f, -1.0f) * particle_size);
		BodyOffsets::cube_grain->push_back(glm::vec3(-1.0f, 1.0f, -1.0f) * particle_size);
		BodyOffsets::cube_grain->push_back(glm::vec3(-1.0f, 1.0f, 1.0f) * particle_size);
		BodyOffsets::cube_grain->push_back(glm::vec3(1.0f, -1.0f, 1.0f) * particle_size);
		BodyOffsets::cube_grain->push_back(glm::vec3(1.0f, -1.0f, -1.0f) * particle_size);
		BodyOffsets::cube_grain->push_back(glm::vec3(-1.0f, -1.0f, -1.0f) * particle_size);
		BodyOffsets::cube_grain->push_back(glm::vec3(-1.0f, -1.0f, 1.0f) * particle_size);
	}

	return BodyOffsets::cube_grain;
}

std::vector<glm::vec3>* BodyOffsets::get_sphere_grain(float particle_size)
{
	if (BodyOffsets::sphere_grain == nullptr)
	{
		BodyOffsets::sphere_grain = new std::vector<glm::vec3>();
		BodyOffsets::sphere_grain->push_back(glm::vec3(0.0f, 0.0f, 0.0f));
	}

	return BodyOffsets::sphere_grain;
}
