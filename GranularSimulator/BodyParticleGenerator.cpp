#include "BodyParticleGenerator.h"
#include "glm/gtc/type_ptr.hpp"

using CodeMonkeys::GranularSimulator::BodyParticleGenerator;

const float BodyParticleGenerator::CUBE_GRAIN_CORNER_PARTICLE_RATIO = 0.3f;
const float BodyParticleGenerator::TETRAHEDRON_GRAIN_PARTICLE_RATIO = 0.5f;

glm::mat3 BodyParticleGenerator::calculate_inertial_moment(std::vector<glm::vec3> particle_offsets, glm::vec3 center_of_mass, float particle_mass_to_size_ratio, std::vector<float> particle_sizes)
{
	float principle_moments_of_inertia[3] = { 0 };
	float products_of_inertia[3] = { 0 };
	for (unsigned int i = 0; i < 3; i++)
	{
		for (unsigned int particle_index = 0; particle_index < particle_offsets.size(); particle_index++)
		{
			float principle_part = particle_mass_to_size_ratio * particle_sizes[particle_index] * glm::pow(particle_offsets[particle_index][(i + 1) % 3] - center_of_mass[(i + 1) % 3], 2.f);
			principle_part	  += particle_mass_to_size_ratio * particle_sizes[particle_index] * glm::pow(particle_offsets[particle_index][(i + 2) % 3] - center_of_mass[(i + 2) % 3], 2.f);
			principle_moments_of_inertia[i] += principle_part;

			float product_part = particle_mass_to_size_ratio * -particle_sizes[particle_index] * (particle_offsets[particle_index][(i + 1) % 3] - center_of_mass[(i + 1) % 3]) * (particle_offsets[particle_index][(i + 2) % 3] - center_of_mass[(i + 2) % 3]);
			products_of_inertia[i] += product_part;
		}
	}
	float mat[9] = {
		principle_moments_of_inertia[0],	products_of_inertia[2],				products_of_inertia[1],
		products_of_inertia[2],				principle_moments_of_inertia[1],	products_of_inertia[0],
		products_of_inertia[1],				products_of_inertia[0],				principle_moments_of_inertia[2] };

	glm::mat3 inertial_moment = glm::transpose(glm::make_mat3(mat));
	return inertial_moment;
}

float BodyParticleGenerator::calculate_total_body_mass(std::vector<float> particle_sizes, float particle_mass_to_size_ratio)
{
	float total_body_mass = 0;
	for (unsigned int i = 0; i < particle_sizes.size(); i++)
	{
		total_body_mass += particle_sizes[i] * particle_mass_to_size_ratio;
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

void BodyParticleGenerator::get_tetrahedron_grain(float base_particle_size, float particle_mass_to_size_ratio, std::vector<glm::vec3>& out_particle_offsets, std::vector<float>& out_particle_sizes, glm::vec3& center_of_mass, float& body_mass, glm::mat3& inertial_moment)
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
	inertial_moment = BodyParticleGenerator::calculate_inertial_moment(out_particle_offsets, center_of_mass, particle_mass_to_size_ratio, out_particle_sizes);
	body_mass = BodyParticleGenerator::calculate_total_body_mass(out_particle_sizes, particle_mass_to_size_ratio);
}

void BodyParticleGenerator::get_cube_grain(float base_particle_size, float particle_mass_to_size_ratio, std::vector<glm::vec3>& out_particle_offsets, std::vector<float>& out_particle_sizes, glm::vec3& center_of_mass, float& body_mass, glm::mat3& inertial_moment)
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
	inertial_moment = BodyParticleGenerator::calculate_inertial_moment(out_particle_offsets, center_of_mass, particle_mass_to_size_ratio, out_particle_sizes);
	body_mass = BodyParticleGenerator::calculate_total_body_mass(out_particle_sizes, particle_mass_to_size_ratio);
}

void BodyParticleGenerator::get_sphere_grain(float base_particle_size, float particle_mass_to_size_ratio, std::vector<glm::vec3>& out_particle_offsets, std::vector<float>& out_particle_sizes, glm::vec3& center_of_mass, float& body_mass, glm::mat3& inertial_moment)
{
	out_particle_offsets.push_back(glm::vec3(0.f, 0.f, 0.f));

	out_particle_sizes.push_back(base_particle_size);

	center_of_mass = BodyParticleGenerator::calculate_center_of_mass(out_particle_offsets);
	inertial_moment = BodyParticleGenerator::calculate_inertial_moment(out_particle_offsets, center_of_mass, particle_mass_to_size_ratio, out_particle_sizes);
	body_mass = BodyParticleGenerator::calculate_total_body_mass(out_particle_sizes, particle_mass_to_size_ratio);
}

//  C
//
//  A    B
//
void BodyParticleGenerator::get_plane(float base_particle_size, float particle_mass_to_size_ratio, glm::vec3 corner_a, glm::vec3 corner_b, glm::vec3 corner_c, std::vector<glm::vec3>& out_particle_offsets, std::vector<float>& out_particle_sizes, glm::vec3& center_of_mass, float& body_mass, glm::mat3& inertial_moment)
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

	inertial_moment = BodyParticleGenerator::calculate_inertial_moment(out_particle_offsets, center_of_mass, particle_mass_to_size_ratio, out_particle_sizes);
	body_mass = BodyParticleGenerator::calculate_total_body_mass(out_particle_sizes, particle_mass_to_size_ratio);
}
