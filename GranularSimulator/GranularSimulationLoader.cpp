#include "GranularSimulationLoader.h"

#include "GranularSubstanceSimulator.h"
#include <fstream>
#include <glm/glm.hpp>
#include <iostream>

using CodeMonkeys::GranularSimulator::GranularSubstanceSimulator;
using namespace CodeMonkeys::GranularSimulator;


void GranularSimulationLoader::save_simulation(std::string filename, GranularSubstanceSimulator* simulator)
{
	std::ofstream file;
	file.open(filename, std::ios::trunc | std::ios::out);

	file << simulator->frame_count << std::endl;
	file << simulator->timestep_size << std::endl;
	file << simulator->particle_count << std::endl;
	file << simulator->body_count << std::endl;
	file << simulator->particle_mass << std::endl;
	file << simulator->kd << std::endl;
	file << simulator->kr << std::endl;
	file << simulator->alpha << std::endl;
	file << simulator->beta << std::endl;
	file << simulator->mu << std::endl;

	file << std::endl << std::endl;

	for (unsigned int particle_index = 0; particle_index < simulator->particle_count; particle_index++)
	{
		file << simulator->particle_sizes[particle_index];
	}

	file << std::endl << std::endl << std::endl;

	for (unsigned int state_index = 0; state_index < simulator->frame_count; state_index++)
	{

		file << simulator->states[state_index].t << std::endl;

		for (unsigned int body_index = 0; body_index < simulator->body_count; body_index++)
		{
			glm::vec3 body_position = simulator->states[state_index].body_positions[body_index];
			file << body_position.x << " " << body_position.y << " " << body_position.z << " ";
		}
		file << std::endl;
		for (unsigned int particle_index = 0; particle_index < simulator->particle_count; particle_index++)
		{
			glm::vec3 particle_position = simulator->states[state_index].particle_positions[particle_index];
			file << particle_position.x << " " << particle_position.y << " " << particle_position.z << " ";
		}
		file << std::endl << std::endl;
	}

	

	file.close();
}

GranularSubstanceSimulator* GranularSimulationLoader::load_simuation(std::string filename)
{
	std::cout << "Loading simulation from file '" << filename << "'" << std::endl << std::endl;
	std::ifstream file;
	file.open(filename, std::ios::in);

	unsigned int frame_count;
	float timestep_size;
	unsigned int particle_count;
	unsigned int body_count;
	float particle_mass;
	float kd;
	float kr;
	float alpha;
	float beta;
	float mu;

	file >> frame_count;
	file >> timestep_size;
	file >> particle_count;
	file >> body_count;
	file >> particle_mass;
	file >> kd;
	file >> kr;
	file >> alpha;
	file >> beta;
	file >> mu;

	GranularSubstanceSimulator* simulator = new GranularSubstanceSimulator(frame_count, timestep_size, particle_mass, kd, kr, alpha, beta, mu);
	
	simulator->particle_count = particle_count;
	simulator->body_count = body_count;
	simulator->particle_sizes = std::vector<float>(particle_count, 0);
	for (unsigned int particle_index = 0; particle_index < particle_count; particle_index++)
	{
		file >> simulator->particle_sizes[particle_index];
	}


	simulator->states = std::vector<State>(frame_count, State(particle_count, body_count));

	for (unsigned int frame_index = 0; frame_index < frame_count; frame_index++)
	{
		file >> simulator->states[frame_index].t;

		for (unsigned int body_index = 0; body_index < body_count; body_index++)
		{
			file >> simulator->states[frame_index].body_positions[body_index].x;
			file >> simulator->states[frame_index].body_positions[body_index].y;
			file >> simulator->states[frame_index].body_positions[body_index].z;
		}

		for (unsigned int particle_index = 0; particle_index < particle_count; particle_index++)
		{
			file >> simulator->states[frame_index].particle_positions[particle_index].x;
			file >> simulator->states[frame_index].particle_positions[particle_index].y;
			file >> simulator->states[frame_index].particle_positions[particle_index].z;
		}
		std::cout << "Loaded frame " << frame_index << " of " << frame_count << " " << 100 * (frame_index / (float)frame_count) << "%" << std::endl;
	}

	
	file.close();

	std::cout << std::endl << std::endl << "Finished loading simulation from file" << std::endl;

	return simulator;
}
