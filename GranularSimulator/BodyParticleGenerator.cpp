#include "BodyParticleGenerator.h"

using CodeMonkeys::GranularSimulator::BodyParticleGenerator;

const float BodyParticleGenerator::CUBE_GRAIN_CORNER_PARTICLE_RATIO = 0.3f;
const float BodyParticleGenerator::TETRAHEDRON_GRAIN_PARTICLE_RATIO = 0.5f;

std::vector<glm::vec3> BodyParticleGenerator::get_tetrahedron_grain_offsets(float base_particle_size)
{
	std::vector<glm::vec3> tetrahedron_grain_offsets = std::vector<glm::vec3>();
	tetrahedron_grain_offsets.push_back(glm::vec3(1.0f, 0.0f, -1.0f / glm::sqrt(2.0f)) * base_particle_size * TETRAHEDRON_GRAIN_PARTICLE_RATIO);
	tetrahedron_grain_offsets.push_back(glm::vec3(-1.0f, 0.0f, -1.0f / glm::sqrt(2.0f)) * base_particle_size * TETRAHEDRON_GRAIN_PARTICLE_RATIO);
	tetrahedron_grain_offsets.push_back(glm::vec3(0.0f, 1.0f, 1.0f / glm::sqrt(2.0f)) * base_particle_size * TETRAHEDRON_GRAIN_PARTICLE_RATIO);
	tetrahedron_grain_offsets.push_back(glm::vec3(0.0f, -1.0f, 1.0f / glm::sqrt(2.0f)) * base_particle_size * TETRAHEDRON_GRAIN_PARTICLE_RATIO);

	return tetrahedron_grain_offsets;
}
std::vector<glm::vec3> BodyParticleGenerator::get_cube_grain_offsets(float base_particle_size)
{
	const float CORNER = 1.0f / glm::sqrt(3.0f);
	std::vector<glm::vec3> cube_grain_offsets = std::vector<glm::vec3>();
	cube_grain_offsets.push_back(glm::vec3(0.0f, 0.0f, 0.0f) * base_particle_size);
	cube_grain_offsets.push_back(glm::vec3(CORNER, CORNER, CORNER) * base_particle_size * (1.0f + CUBE_GRAIN_CORNER_PARTICLE_RATIO));
	cube_grain_offsets.push_back(glm::vec3(CORNER, CORNER, -CORNER) * base_particle_size * (1.0f + CUBE_GRAIN_CORNER_PARTICLE_RATIO));
	cube_grain_offsets.push_back(glm::vec3(-CORNER, CORNER, -CORNER) * base_particle_size * (1.0f + CUBE_GRAIN_CORNER_PARTICLE_RATIO));
	cube_grain_offsets.push_back(glm::vec3(-CORNER, CORNER, CORNER) * base_particle_size * (1.0f + CUBE_GRAIN_CORNER_PARTICLE_RATIO));
	cube_grain_offsets.push_back(glm::vec3(CORNER, -CORNER, CORNER) * base_particle_size * (1.0f + CUBE_GRAIN_CORNER_PARTICLE_RATIO));
	cube_grain_offsets.push_back(glm::vec3(CORNER, -CORNER, -CORNER) * base_particle_size * (1.0f + CUBE_GRAIN_CORNER_PARTICLE_RATIO));
	cube_grain_offsets.push_back(glm::vec3(-CORNER, -CORNER, -CORNER) * base_particle_size * (1.0f + CUBE_GRAIN_CORNER_PARTICLE_RATIO));
	cube_grain_offsets.push_back(glm::vec3(-CORNER, -CORNER, CORNER) * base_particle_size * (1.0f + CUBE_GRAIN_CORNER_PARTICLE_RATIO));

	return cube_grain_offsets;
}

std::vector<glm::vec3> BodyParticleGenerator::get_sphere_grain_offsets(float base_particle_size)
{
	std::vector<glm::vec3> sphere_grain_offsets = std::vector<glm::vec3>();
	sphere_grain_offsets.push_back(glm::vec3(0.0f, 0.0f, 0.0f));

	return sphere_grain_offsets;
}

std::vector<float> BodyParticleGenerator::get_tetrahedron_grain_sizes(float base_particle_size)
{
	std::vector<float> tetrahedron_grain_sizes = std::vector<float>();
	tetrahedron_grain_sizes.push_back(base_particle_size * TETRAHEDRON_GRAIN_PARTICLE_RATIO);
	tetrahedron_grain_sizes.push_back(base_particle_size * TETRAHEDRON_GRAIN_PARTICLE_RATIO);
	tetrahedron_grain_sizes.push_back(base_particle_size * TETRAHEDRON_GRAIN_PARTICLE_RATIO);
	tetrahedron_grain_sizes.push_back(base_particle_size * TETRAHEDRON_GRAIN_PARTICLE_RATIO);

	return tetrahedron_grain_sizes;
}
std::vector<float> BodyParticleGenerator::get_cube_grain_sizes(float base_particle_size)
{
	std::vector<float> cube_grain_sizes = std::vector<float>();
	cube_grain_sizes.push_back(base_particle_size);
	cube_grain_sizes.push_back(base_particle_size * CUBE_GRAIN_CORNER_PARTICLE_RATIO);
	cube_grain_sizes.push_back(base_particle_size * CUBE_GRAIN_CORNER_PARTICLE_RATIO);
	cube_grain_sizes.push_back(base_particle_size * CUBE_GRAIN_CORNER_PARTICLE_RATIO);
	cube_grain_sizes.push_back(base_particle_size * CUBE_GRAIN_CORNER_PARTICLE_RATIO);
	cube_grain_sizes.push_back(base_particle_size * CUBE_GRAIN_CORNER_PARTICLE_RATIO);
	cube_grain_sizes.push_back(base_particle_size * CUBE_GRAIN_CORNER_PARTICLE_RATIO);
	cube_grain_sizes.push_back(base_particle_size * CUBE_GRAIN_CORNER_PARTICLE_RATIO);
	cube_grain_sizes.push_back(base_particle_size * CUBE_GRAIN_CORNER_PARTICLE_RATIO);

	return cube_grain_sizes;
}

std::vector<float> BodyParticleGenerator::get_sphere_grain_sizes(float base_particle_size)
{
	std::vector<float> sphere_grain_sizes = std::vector<float>();
	sphere_grain_sizes.push_back(base_particle_size);

	return sphere_grain_sizes;
}
