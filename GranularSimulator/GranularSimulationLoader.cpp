#include "GranularSimulationLoader.h"

#include "State.h"
#include "GranularSubstanceSimulator.h"
#include "Body.h"
#include "Particle.h"
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <iostream>

using CodeMonkeys::GranularSimulator::GranularSubstanceSimulator;
using namespace CodeMonkeys::GranularSimulator;


void GranularSimulationLoader::save_simulation(std::string filename, GranularSubstanceSimulator* simulator)
{
	std::ofstream file;
	file.open(filename, std::ios::trunc | std::ios::out);

	file << simulator->frame_count << std::endl;
	file << simulator->framerate << std::endl;
	file << simulator->initial_timestep_size << std::endl;
	file << simulator->particles.size() << std::endl;
	file << simulator->bodies.size() << std::endl;
	file << simulator->particle_density << std::endl;
	file << simulator->kd << std::endl;
	file << simulator->kr << std::endl;
	file << simulator->alpha << std::endl;
	file << simulator->beta << std::endl;
	file << simulator->mu << std::endl;

	file << std::endl << std::endl;

	for (unsigned int body_index = 0; body_index < simulator->bodies.size(); body_index++)
	{
		file << simulator->bodies[body_index].total_mass << " " << simulator->bodies[body_index].is_movable << " " << simulator->bodies[body_index].is_visible << " " << simulator->bodies[body_index].particle_indices.size() << " ";
		for (body_particle_index_it it = simulator->bodies[body_index].particle_indices.begin(); it != simulator->bodies[body_index].particle_indices.end(); it++)
		{
			file << (*it) << " ";
		}
	}

	file << std::endl << std::endl << std::endl << std::endl;

	for (unsigned int particle_index = 0; particle_index < simulator->particles.size(); particle_index++)
	{
		glm::vec3 offset = simulator->particles[particle_index].offset_from_body;
		file << simulator->particles[particle_index].radius << " " << offset.x << " " << offset.y << " " << offset.z << " ";
	}

	file << std::endl << std::endl << std::endl << std::endl;

	for (unsigned int state_index = 0; state_index < simulator->frame_count; state_index++)
	{
		file << simulator->frame_states[state_index].t << std::endl;

		for (unsigned int body_index = 0; body_index < simulator->bodies.size(); body_index++)
		{
			glm::vec3 body_position = simulator->frame_states[state_index].body_positions[body_index];
			file << body_position.x << " " << body_position.y << " " << body_position.z << " ";

			glm::quat body_orientation = simulator->frame_states[state_index].body_orientations[body_index];
			file << body_orientation.x << " " << body_orientation.y << " " << body_orientation.z << " " << body_orientation.w << " ";
		}
		file << std::endl << std::endl;
		for (unsigned int particle_index = 0; particle_index < simulator->particles.size(); particle_index++)
		{
			glm::vec3 particle_position = simulator->frame_states[state_index].particle_positions[particle_index];
			file << particle_position.x << " " << particle_position.y << " " << particle_position.z << " ";
		}
		file << std::endl << std::endl << std::endl;
	}

	

	file.close();
}

GranularSubstanceSimulator* GranularSimulationLoader::load_simuation(std::string filename)
{
	std::cout << "Loading simulation from file '" << filename << "'" << std::endl << std::endl;
	std::ifstream file;
	file.open(filename, std::ios::in);

	unsigned int frame_count;
	float framerate;
	float timestep_size;
	unsigned int particle_count;
	unsigned int body_count;
	float particle_density;
	float kd;
	float kr;
	float alpha;
	float beta;
	float mu;

	file >> frame_count;
	file >> framerate;
	file >> timestep_size;
	file >> particle_count;
	file >> body_count;
	file >> particle_density;
	file >> kd;
	file >> kr;
	file >> alpha;
	file >> beta;
	file >> mu;

	GranularSubstanceSimulator* simulator = new GranularSubstanceSimulator((1.f / framerate) * frame_count, framerate, timestep_size, particle_density, kd, kr, alpha, beta, mu);
	
	simulator->bodies.clear();
	for (unsigned int body_index = 0; body_index < body_count; body_index++)
	{
		unsigned int particle_count;
		float total_mass;
		bool is_movable;
		bool is_visible;
		file >> total_mass;
		file >> is_movable;
		file >> is_visible;
		file >> particle_count;

		Body body = Body(total_mass, glm::mat3(1.f), is_movable, is_visible);
		for (unsigned int i = 0; i < particle_count; i++)
		{
			unsigned int particle_index;
			file >> particle_index;
			body.particle_indices.insert(particle_index);
		}
		simulator->bodies.push_back(body);
	}


	simulator->particles.clear();
	for (unsigned int particle_index = 0; particle_index < particle_count; particle_index++)
	{
		float radius;
		glm::vec3 offset;
		file >> radius;
		file >> offset.x;
		file >> offset.y;
		file >> offset.z;
		simulator->particles.push_back(Particle(radius, offset));
	}

	for (unsigned int body_index = 0; body_index < body_count; body_index++)
	{
		for (body_particle_index_it it = simulator->bodies[body_index].particle_indices.begin(); it != simulator->bodies[body_index].particle_indices.end(); it++)
		{
			simulator->particles[(*it)].body_index = body_index;
		}
	}

	simulator->frame_states = std::vector<State>(frame_count, State(particle_count, body_count));

	for (unsigned int frame_index = 0; frame_index < frame_count; frame_index++)
	{
		file >> simulator->frame_states[frame_index].t;

		for (unsigned int body_index = 0; body_index < body_count; body_index++)
		{
			file >> simulator->frame_states[frame_index].body_positions[body_index].x;
			file >> simulator->frame_states[frame_index].body_positions[body_index].y;
			file >> simulator->frame_states[frame_index].body_positions[body_index].z;

			file >> simulator->frame_states[frame_index].body_orientations[body_index].x;
			file >> simulator->frame_states[frame_index].body_orientations[body_index].y;
			file >> simulator->frame_states[frame_index].body_orientations[body_index].z;
			file >> simulator->frame_states[frame_index].body_orientations[body_index].w;
		}

		for (unsigned int particle_index = 0; particle_index < particle_count; particle_index++)
		{
			file >> simulator->frame_states[frame_index].particle_positions[particle_index].x;
			file >> simulator->frame_states[frame_index].particle_positions[particle_index].y;
			file >> simulator->frame_states[frame_index].particle_positions[particle_index].z;
		}
		std::cout << "Loaded frame " << frame_index << " of " << frame_count << " " << 100 * (frame_index / (float)frame_count) << "%" << std::endl;
	}

	
	file.close();

	std::cout << std::endl << std::endl << "Finished loading simulation from file" << std::endl;

	return simulator;
}
