#include "BodyGenerator.h"

#include "Body.h"
#include "Particle.h"
#include "glm/gtc/type_ptr.hpp"

using CodeMonkeys::GranularSimulator::BodyGenerator;

using namespace CodeMonkeys::GranularSimulator;


const float BodyGenerator::CUBE_GRAIN_CORNER_PARTICLE_RATIO = 0.3f;
const float BodyGenerator::TETRAHEDRON_GRAIN_PARTICLE_RATIO = 0.5f;

float BodyGenerator::calculate_particle_mass(float particle_radius, float particle_density)
{
	float particle_volume = (4.f / 3.f) * 3.141592f * glm::pow(particle_radius, 3.f);
	float particle_mass = particle_volume * particle_density;

	return particle_mass;
}

// Moment of inertia tensor for sphere:
// https://en.wikipedia.org/wiki/List_of_moments_of_inertia#List_of_3D_inertia_tensors
//
// Shift moment of inertia tensor by offset:
// https://www.physicsforums.com/threads/parallel-axis-theorem-and-interia-tensors.276083/
// http://www.dartmouth.edu/~phys44/lectures/Chap_7.pdf
glm::mat3 BodyGenerator::calculate_inverse_inertial_moment(std::vector<Particle> particles, glm::vec3 center_of_mass, float particle_density)
{
	float principle_moments_of_inertia[3] = { 0 };
	float products_of_inertia[3] = { 0 };


	glm::mat3 inertial_moment = glm::mat3(0.f);
	for (unsigned int i = 0; i < particles.size(); i++)
	{
		float particle_mass = BodyGenerator::calculate_particle_mass(particles[i].radius, particle_density);
		glm::mat3 particle_inertial_moment = glm::mat3(1.f) * (2.f / 5.f) * particle_mass * glm::pow(particles[i].radius, 2.f);

		glm::vec3 offset = particles[i].offset_from_body;
		float offset_squared = glm::dot(offset, offset);
		glm::mat3 offset_particle_inertial_moment = particle_inertial_moment + particle_mass * (offset_squared * glm::mat3(1.f) - glm::outerProduct(offset, offset));
		inertial_moment += offset_particle_inertial_moment;
	}

	inertial_moment = glm::inverse(inertial_moment);
	return inertial_moment;
}

float BodyGenerator::calculate_total_body_mass(std::vector<Particle> particles, float particle_density)
{
	float total_body_mass = 0;
	for (unsigned int i = 0; i < particles.size(); i++)
	{
		total_body_mass += BodyGenerator::calculate_particle_mass(particles[i].radius, particle_density);
	}

	return total_body_mass;
}

glm::vec3 BodyGenerator::calculate_center_of_mass(std::vector<Particle> particles)
{
	glm::vec3 center_of_mass = glm::vec3();
	for (unsigned int i = 0; i < particles.size(); i++)
	{
		center_of_mass += particles[i].offset_from_body;
	}

	return center_of_mass / (float)particles.size();
}

glm::vec3 BodyGenerator::generate_tetrahedron_grain(float base_particle_radius, float particle_density, Body& out_body, std::vector<Particle>& out_particles)
{
	out_particles.push_back(Particle(base_particle_radius * TETRAHEDRON_GRAIN_PARTICLE_RATIO, glm::vec3(1.f, 0.f, -1.f / glm::sqrt(2.f)) * base_particle_radius * TETRAHEDRON_GRAIN_PARTICLE_RATIO));
	out_particles.push_back(Particle(base_particle_radius * TETRAHEDRON_GRAIN_PARTICLE_RATIO, glm::vec3(-1.f, 0.f, -1.f / glm::sqrt(2.f)) * base_particle_radius * TETRAHEDRON_GRAIN_PARTICLE_RATIO));
	out_particles.push_back(Particle(base_particle_radius * TETRAHEDRON_GRAIN_PARTICLE_RATIO, glm::vec3(0.f, 1.f, 1.f / glm::sqrt(2.f)) * base_particle_radius * TETRAHEDRON_GRAIN_PARTICLE_RATIO));
	out_particles.push_back(Particle(base_particle_radius * TETRAHEDRON_GRAIN_PARTICLE_RATIO, glm::vec3(0.f, -1.f, 1.f / glm::sqrt(2.f)) * base_particle_radius * TETRAHEDRON_GRAIN_PARTICLE_RATIO));

	glm::vec3 center_of_mass = BodyGenerator::calculate_center_of_mass(out_particles);
	glm::mat3 inertial_moment = BodyGenerator::calculate_inverse_inertial_moment(out_particles, center_of_mass, particle_density);
	float body_mass = BodyGenerator::calculate_total_body_mass(out_particles, particle_density);

	out_body = Body(body_mass, inertial_moment);

	return center_of_mass;
}

glm::vec3 BodyGenerator::generate_cube_grain(float base_particle_radius, float particle_density, Body& out_body, std::vector<Particle>& out_particles)
{
	const float CORNER = 1.f / glm::sqrt(3.f);
	out_particles.push_back(Particle(base_particle_radius, glm::vec3(0.f, 0.f, 0.f) * base_particle_radius));
	out_particles.push_back(Particle(base_particle_radius * CUBE_GRAIN_CORNER_PARTICLE_RATIO, glm::vec3(CORNER, CORNER, CORNER) * base_particle_radius * (1.f + CUBE_GRAIN_CORNER_PARTICLE_RATIO)));
	out_particles.push_back(Particle(base_particle_radius * CUBE_GRAIN_CORNER_PARTICLE_RATIO, glm::vec3(CORNER, CORNER, -CORNER) * base_particle_radius * (1.f + CUBE_GRAIN_CORNER_PARTICLE_RATIO)));
	out_particles.push_back(Particle(base_particle_radius * CUBE_GRAIN_CORNER_PARTICLE_RATIO, glm::vec3(-CORNER, CORNER, -CORNER) * base_particle_radius * (1.f + CUBE_GRAIN_CORNER_PARTICLE_RATIO)));
	out_particles.push_back(Particle(base_particle_radius * CUBE_GRAIN_CORNER_PARTICLE_RATIO, glm::vec3(-CORNER, CORNER, CORNER) * base_particle_radius * (1.f + CUBE_GRAIN_CORNER_PARTICLE_RATIO)));
	out_particles.push_back(Particle(base_particle_radius * CUBE_GRAIN_CORNER_PARTICLE_RATIO, glm::vec3(CORNER, -CORNER, CORNER) * base_particle_radius * (1.f + CUBE_GRAIN_CORNER_PARTICLE_RATIO)));
	out_particles.push_back(Particle(base_particle_radius * CUBE_GRAIN_CORNER_PARTICLE_RATIO, glm::vec3(CORNER, -CORNER, -CORNER) * base_particle_radius * (1.f + CUBE_GRAIN_CORNER_PARTICLE_RATIO)));
	out_particles.push_back(Particle(base_particle_radius * CUBE_GRAIN_CORNER_PARTICLE_RATIO, glm::vec3(-CORNER, -CORNER, -CORNER) * base_particle_radius * (1.f + CUBE_GRAIN_CORNER_PARTICLE_RATIO)));
	out_particles.push_back(Particle(base_particle_radius * CUBE_GRAIN_CORNER_PARTICLE_RATIO, glm::vec3(-CORNER, -CORNER, CORNER) * base_particle_radius * (1.f + CUBE_GRAIN_CORNER_PARTICLE_RATIO)));

	glm::vec3 center_of_mass = BodyGenerator::calculate_center_of_mass(out_particles);
	glm::mat3 inertial_moment = BodyGenerator::calculate_inverse_inertial_moment(out_particles, center_of_mass, particle_density);
	float body_mass = BodyGenerator::calculate_total_body_mass(out_particles, particle_density);

	out_body = Body(body_mass, inertial_moment);
	return center_of_mass;
}

glm::vec3 BodyGenerator::generate_sphere_grain(float base_particle_radius, float particle_density, Body& out_body, std::vector<Particle>& out_particles)
{
	out_particles.push_back(Particle(base_particle_radius, glm::vec3(0.f, 0.f, 0.f)));

	glm::vec3 center_of_mass = BodyGenerator::calculate_center_of_mass(out_particles);
	glm::mat3 inertial_moment = BodyGenerator::calculate_inverse_inertial_moment(out_particles, center_of_mass, particle_density);
	float body_mass = BodyGenerator::calculate_total_body_mass(out_particles, particle_density);

	out_body = Body(body_mass, inertial_moment);
	return center_of_mass;
}

//  C
//
//  A    B
//
glm::vec3 BodyGenerator::generate_plane(float base_particle_radius, float particle_density, glm::vec3 corner_a, glm::vec3 corner_b, glm::vec3 corner_c, Body& out_body, std::vector<Particle>& out_particles)
{
	unsigned int plane_particles_width = (unsigned int)(glm::length(corner_b - corner_a) / (base_particle_radius * 2));
	unsigned int plane_particles_height = (unsigned int)(glm::length(corner_c - corner_a) / (base_particle_radius * 2));
	glm::vec3 x_direction = glm::normalize(corner_b - corner_a);
	glm::vec3 y_direction = glm::normalize(corner_c - corner_a);

	for (unsigned int y = 0; y < plane_particles_height; y++)
	{
		for (unsigned int x = 0; x < plane_particles_width; x++)
		{
			out_particles.push_back(Particle(base_particle_radius, corner_a + x_direction * base_particle_radius * (2.f * x) + y_direction * base_particle_radius * (2.f * y)));
		}
	}

	glm::vec3 center_of_mass = BodyGenerator::calculate_center_of_mass(out_particles);

	for (unsigned int i = 0; i < out_particles.size(); i++)
	{
		out_particles[i].offset_from_body -= center_of_mass;
	}

	glm::mat3 inertial_moment = BodyGenerator::calculate_inverse_inertial_moment(out_particles, center_of_mass, particle_density);
	float body_mass = BodyGenerator::calculate_total_body_mass(out_particles, particle_density);

	out_body = Body(body_mass, inertial_moment);

	return center_of_mass;
}
