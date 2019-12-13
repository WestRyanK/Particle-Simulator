#include "BodyParticleGenerator.h"
#include "glm/gtc/type_ptr.hpp"

using CodeMonkeys::GranularSimulator::BodyParticleGenerator;

const float BodyParticleGenerator::CUBE_GRAIN_CORNER_PARTICLE_RATIO = 0.3f;
const float BodyParticleGenerator::TETRAHEDRON_GRAIN_PARTICLE_RATIO = 0.5f;

float BodyParticleGenerator::calculate_particle_mass(float particle_size, float particle_density)
{
	float particle_volume = (4.f / 3.f) * 3.141592f * glm::pow(particle_size, 3.f);
	float particle_mass = particle_volume * particle_density;

	return particle_mass;
}

// Moment of inertia tensor for sphere:
// https://en.wikipedia.org/wiki/List_of_moments_of_inertia#List_of_3D_inertia_tensors
//
// Shift moment of inertia tensor by offset:
// https://www.physicsforums.com/threads/parallel-axis-theorem-and-interia-tensors.276083/
// http://www.dartmouth.edu/~phys44/lectures/Chap_7.pdf
glm::mat3 BodyParticleGenerator::calculate_inertial_moment(std::vector<glm::vec3> particle_offsets, glm::vec3 center_of_mass, float particle_density, std::vector<float> particle_sizes)
{
	float principle_moments_of_inertia[3] = { 0 };
	float products_of_inertia[3] = { 0 };


	glm::mat3 inertial_moment = glm::mat3(0.f);
	for (unsigned int i = 0; i < particle_offsets.size(); i++)
	{
		float particle_mass = BodyParticleGenerator::calculate_particle_mass(particle_sizes[i], particle_density);
		glm::mat3 particle_inertial_moment = glm::mat3(1.f) * (2.f / 5.f) * particle_mass * glm::pow(particle_sizes[i], 2.f);

		glm::vec3 offset = particle_offsets[i];
		float offset_squared = glm::dot(offset, offset);
		glm::mat3 offset_particle_inertial_moment = particle_inertial_moment + particle_mass * (offset_squared * glm::mat3(1.f) - glm::outerProduct(offset, offset));
		inertial_moment += offset_particle_inertial_moment;
	}

	return inertial_moment;
}

float BodyParticleGenerator::calculate_total_body_mass(std::vector<float> particle_sizes, float particle_density)
{
	float total_body_mass = 0;
	for (unsigned int i = 0; i < particle_sizes.size(); i++)
	{
		total_body_mass += BodyParticleGenerator::calculate_particle_mass(particle_sizes[i], particle_density);
	}

	return total_body_mass;
}

glm::vec3 BodyParticleGenerator::calculate_center_of_mass(std::vector<glm::vec3> particle_offsets)
{
	glm::vec3 center_of_mass = glm::vec3();
	for (unsigned int i = 0; i < particle_offsets.size(); i++)
	{
		center_of_mass += particle_offsets[i];
	}

	return center_of_mass / (float)particle_offsets.size();
}

void BodyParticleGenerator::get_tetrahedron_grain(float base_particle_size, float particle_density, std::vector<glm::vec3>& out_particle_offsets, std::vector<float>& out_particle_sizes, glm::vec3& center_of_mass, float& body_mass, glm::mat3& inertial_moment)
{
	out_particle_offsets.push_back(glm::vec3(1.f, 0.f, -1.f / glm::sqrt(2.f)) * base_particle_size * TETRAHEDRON_GRAIN_PARTICLE_RATIO);
	out_particle_offsets.push_back(glm::vec3(-1.f, 0.f, -1.f / glm::sqrt(2.f)) * base_particle_size * TETRAHEDRON_GRAIN_PARTICLE_RATIO);
	out_particle_offsets.push_back(glm::vec3(0.f, 1.f, 1.f / glm::sqrt(2.f)) * base_particle_size * TETRAHEDRON_GRAIN_PARTICLE_RATIO);
	out_particle_offsets.push_back(glm::vec3(0.f, -1.f, 1.f / glm::sqrt(2.f)) * base_particle_size * TETRAHEDRON_GRAIN_PARTICLE_RATIO);

	out_particle_sizes.push_back(base_particle_size * TETRAHEDRON_GRAIN_PARTICLE_RATIO);
	out_particle_sizes.push_back(base_particle_size * TETRAHEDRON_GRAIN_PARTICLE_RATIO);
	out_particle_sizes.push_back(base_particle_size * TETRAHEDRON_GRAIN_PARTICLE_RATIO);
	out_particle_sizes.push_back(base_particle_size * TETRAHEDRON_GRAIN_PARTICLE_RATIO);

	center_of_mass = BodyParticleGenerator::calculate_center_of_mass(out_particle_offsets);
	inertial_moment = BodyParticleGenerator::calculate_inertial_moment(out_particle_offsets, center_of_mass, particle_density, out_particle_sizes);
	body_mass = BodyParticleGenerator::calculate_total_body_mass(out_particle_sizes, particle_density);
}

void BodyParticleGenerator::get_cube_grain(float base_particle_size, float particle_density, std::vector<glm::vec3>& out_particle_offsets, std::vector<float>& out_particle_sizes, glm::vec3& center_of_mass, float& body_mass, glm::mat3& inertial_moment)
{
	const float CORNER = 1.f / glm::sqrt(3.f);
	out_particle_offsets.push_back(glm::vec3(0.f, 0.f, 0.f) * base_particle_size);
	out_particle_offsets.push_back(glm::vec3(CORNER, CORNER, CORNER) * base_particle_size * (1.f + CUBE_GRAIN_CORNER_PARTICLE_RATIO));
	out_particle_offsets.push_back(glm::vec3(CORNER, CORNER, -CORNER) * base_particle_size * (1.f + CUBE_GRAIN_CORNER_PARTICLE_RATIO));
	out_particle_offsets.push_back(glm::vec3(-CORNER, CORNER, -CORNER) * base_particle_size * (1.f + CUBE_GRAIN_CORNER_PARTICLE_RATIO));
	out_particle_offsets.push_back(glm::vec3(-CORNER, CORNER, CORNER) * base_particle_size * (1.f + CUBE_GRAIN_CORNER_PARTICLE_RATIO));
	out_particle_offsets.push_back(glm::vec3(CORNER, -CORNER, CORNER) * base_particle_size * (1.f + CUBE_GRAIN_CORNER_PARTICLE_RATIO));
	out_particle_offsets.push_back(glm::vec3(CORNER, -CORNER, -CORNER) * base_particle_size * (1.f + CUBE_GRAIN_CORNER_PARTICLE_RATIO));
	out_particle_offsets.push_back(glm::vec3(-CORNER, -CORNER, -CORNER) * base_particle_size * (1.f + CUBE_GRAIN_CORNER_PARTICLE_RATIO));
	out_particle_offsets.push_back(glm::vec3(-CORNER, -CORNER, CORNER) * base_particle_size * (1.f + CUBE_GRAIN_CORNER_PARTICLE_RATIO));

	out_particle_sizes.push_back(base_particle_size);
	out_particle_sizes.push_back(base_particle_size * CUBE_GRAIN_CORNER_PARTICLE_RATIO);
	out_particle_sizes.push_back(base_particle_size * CUBE_GRAIN_CORNER_PARTICLE_RATIO);
	out_particle_sizes.push_back(base_particle_size * CUBE_GRAIN_CORNER_PARTICLE_RATIO);
	out_particle_sizes.push_back(base_particle_size * CUBE_GRAIN_CORNER_PARTICLE_RATIO);
	out_particle_sizes.push_back(base_particle_size * CUBE_GRAIN_CORNER_PARTICLE_RATIO);
	out_particle_sizes.push_back(base_particle_size * CUBE_GRAIN_CORNER_PARTICLE_RATIO);
	out_particle_sizes.push_back(base_particle_size * CUBE_GRAIN_CORNER_PARTICLE_RATIO);
	out_particle_sizes.push_back(base_particle_size * CUBE_GRAIN_CORNER_PARTICLE_RATIO);

	center_of_mass = BodyParticleGenerator::calculate_center_of_mass(out_particle_offsets);
	inertial_moment = BodyParticleGenerator::calculate_inertial_moment(out_particle_offsets, center_of_mass, particle_density, out_particle_sizes);
	body_mass = BodyParticleGenerator::calculate_total_body_mass(out_particle_sizes, particle_density);
}

void BodyParticleGenerator::get_sphere_grain(float base_particle_size, float particle_density, std::vector<glm::vec3>& out_particle_offsets, std::vector<float>& out_particle_sizes, glm::vec3& center_of_mass, float& body_mass, glm::mat3& inertial_moment)
{
	out_particle_offsets.push_back(glm::vec3(0.f, 0.f, 0.f));

	out_particle_sizes.push_back(base_particle_size);

	center_of_mass = BodyParticleGenerator::calculate_center_of_mass(out_particle_offsets);
	inertial_moment = BodyParticleGenerator::calculate_inertial_moment(out_particle_offsets, center_of_mass, particle_density, out_particle_sizes);
	body_mass = BodyParticleGenerator::calculate_total_body_mass(out_particle_sizes, particle_density);
}

//  C
//
//  A    B
//
void BodyParticleGenerator::get_plane(float base_particle_size, float particle_density, glm::vec3 corner_a, glm::vec3 corner_b, glm::vec3 corner_c, std::vector<glm::vec3>& out_particle_offsets, std::vector<float>& out_particle_sizes, glm::vec3& center_of_mass, float& body_mass, glm::mat3& inertial_moment)
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

	center_of_mass = BodyParticleGenerator::calculate_center_of_mass(out_particle_offsets);

	for (unsigned int i = 0; i < out_particle_offsets.size(); i++)
	{
		out_particle_offsets[i] -= center_of_mass;
	}

	inertial_moment = BodyParticleGenerator::calculate_inertial_moment(out_particle_offsets, center_of_mass, particle_density, out_particle_sizes);
	body_mass = BodyParticleGenerator::calculate_total_body_mass(out_particle_sizes, particle_density);
}
