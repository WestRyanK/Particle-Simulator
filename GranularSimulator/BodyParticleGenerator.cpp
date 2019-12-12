#include "BodyParticleGenerator.h"

using CodeMonkeys::GranularSimulator::BodyParticleGenerator;

const float BodyParticleGenerator::CUBE_GRAIN_CORNER_PARTICLE_RATIO = 0.3f;
const float BodyParticleGenerator::TETRAHEDRON_GRAIN_PARTICLE_RATIO = 0.5f;


void BodyParticleGenerator::get_tetrahedron_grain(float base_particle_size, std::vector<glm::vec3>& out_particle_offsets, std::vector<float>& out_particle_sizes)
{
	out_particle_offsets.push_back(glm::vec3(1.0f, 0.0f, -1.0f / glm::sqrt(2.0f)) * base_particle_size * TETRAHEDRON_GRAIN_PARTICLE_RATIO);
	out_particle_offsets.push_back(glm::vec3(-1.0f, 0.0f, -1.0f / glm::sqrt(2.0f)) * base_particle_size * TETRAHEDRON_GRAIN_PARTICLE_RATIO);
	out_particle_offsets.push_back(glm::vec3(0.0f, 1.0f, 1.0f / glm::sqrt(2.0f)) * base_particle_size * TETRAHEDRON_GRAIN_PARTICLE_RATIO);
	out_particle_offsets.push_back(glm::vec3(0.0f, -1.0f, 1.0f / glm::sqrt(2.0f)) * base_particle_size * TETRAHEDRON_GRAIN_PARTICLE_RATIO);

	out_particle_sizes.push_back(base_particle_size * TETRAHEDRON_GRAIN_PARTICLE_RATIO);
	out_particle_sizes.push_back(base_particle_size * TETRAHEDRON_GRAIN_PARTICLE_RATIO);
	out_particle_sizes.push_back(base_particle_size * TETRAHEDRON_GRAIN_PARTICLE_RATIO);
	out_particle_sizes.push_back(base_particle_size * TETRAHEDRON_GRAIN_PARTICLE_RATIO);
}

void BodyParticleGenerator::get_cube_grain(float base_particle_size, std::vector<glm::vec3>& out_particle_offsets, std::vector<float>& out_particle_sizes)
{
	const float CORNER = 1.0f / glm::sqrt(3.0f);
	out_particle_offsets.push_back(glm::vec3(0.0f, 0.0f, 0.0f) * base_particle_size);
	out_particle_offsets.push_back(glm::vec3(CORNER, CORNER, CORNER) * base_particle_size * (1.0f + CUBE_GRAIN_CORNER_PARTICLE_RATIO));
	out_particle_offsets.push_back(glm::vec3(CORNER, CORNER, -CORNER) * base_particle_size * (1.0f + CUBE_GRAIN_CORNER_PARTICLE_RATIO));
	out_particle_offsets.push_back(glm::vec3(-CORNER, CORNER, -CORNER) * base_particle_size * (1.0f + CUBE_GRAIN_CORNER_PARTICLE_RATIO));
	out_particle_offsets.push_back(glm::vec3(-CORNER, CORNER, CORNER) * base_particle_size * (1.0f + CUBE_GRAIN_CORNER_PARTICLE_RATIO));
	out_particle_offsets.push_back(glm::vec3(CORNER, -CORNER, CORNER) * base_particle_size * (1.0f + CUBE_GRAIN_CORNER_PARTICLE_RATIO));
	out_particle_offsets.push_back(glm::vec3(CORNER, -CORNER, -CORNER) * base_particle_size * (1.0f + CUBE_GRAIN_CORNER_PARTICLE_RATIO));
	out_particle_offsets.push_back(glm::vec3(-CORNER, -CORNER, -CORNER) * base_particle_size * (1.0f + CUBE_GRAIN_CORNER_PARTICLE_RATIO));
	out_particle_offsets.push_back(glm::vec3(-CORNER, -CORNER, CORNER) * base_particle_size * (1.0f + CUBE_GRAIN_CORNER_PARTICLE_RATIO));

	out_particle_sizes.push_back(base_particle_size);
	out_particle_sizes.push_back(base_particle_size * CUBE_GRAIN_CORNER_PARTICLE_RATIO);
	out_particle_sizes.push_back(base_particle_size * CUBE_GRAIN_CORNER_PARTICLE_RATIO);
	out_particle_sizes.push_back(base_particle_size * CUBE_GRAIN_CORNER_PARTICLE_RATIO);
	out_particle_sizes.push_back(base_particle_size * CUBE_GRAIN_CORNER_PARTICLE_RATIO);
	out_particle_sizes.push_back(base_particle_size * CUBE_GRAIN_CORNER_PARTICLE_RATIO);
	out_particle_sizes.push_back(base_particle_size * CUBE_GRAIN_CORNER_PARTICLE_RATIO);
	out_particle_sizes.push_back(base_particle_size * CUBE_GRAIN_CORNER_PARTICLE_RATIO);
	out_particle_sizes.push_back(base_particle_size * CUBE_GRAIN_CORNER_PARTICLE_RATIO);
}

void BodyParticleGenerator::get_sphere_grain(float base_particle_size, std::vector<glm::vec3>& out_particle_offsets, std::vector<float>& out_particle_sizes)
{
	out_particle_offsets.push_back(glm::vec3(0.0f, 0.0f, 0.0f));

	out_particle_sizes.push_back(base_particle_size);
}

//  C
//
//  A    B
//
void BodyParticleGenerator::get_plane(float base_particle_size, glm::vec3 corner_a, glm::vec3 corner_b, glm::vec3 corner_c, std::vector<glm::vec3>& out_particle_offsets, std::vector<float>& out_particle_sizes)
{
	unsigned int plane_particles_width = (unsigned int)(glm::length(corner_b - corner_a) / (base_particle_size * 2));
	unsigned int plane_particles_height = (unsigned int)(glm::length(corner_c - corner_a) / (base_particle_size * 2));
	glm::vec3 x_direction = glm::normalize(corner_b - corner_a);
	glm::vec3 y_direction = glm::normalize(corner_c - corner_a);

	for (unsigned int y = 0; y < plane_particles_height; y++)
	{
		for (unsigned int x = 0; x < plane_particles_width; x++)
		{
			out_particle_offsets.push_back(corner_a + x_direction * base_particle_size * (2.f * x) + y_direction * base_particle_size * (2.f * y));
			out_particle_sizes.push_back(base_particle_size);
		}
	}
}
