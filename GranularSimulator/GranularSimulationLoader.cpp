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

	file << std::endl;
	for (unsigned int frame_index = 0; frame_index < simulator->frame_count; frame_index++)
	{
		for (unsigned int body_index = 0; body_index < simulator->body_count; body_index++)
		{
			glm::vec3 body_position = simulator->body_positions[frame_index][body_index];
			file << body_position.x << " " << body_position.y << " " << body_position.z << " ";
		}
		//file << std::endl;
	}

	file.close();
}

GranularSubstanceSimulator* GranularSimulationLoader::load_simuation(std::string filename)
{
	std::ifstream file;
	file.open(filename, std::ios::in);

	unsigned int frame_count;
	float timestep_size;
	unsigned int particle_count;
	unsigned int body_count;
	float particle_size;
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

	for (unsigned int frame_index = 0; frame_index < frame_count; frame_index++)
	{
		simulator->body_positions[frame_index] = std::vector<glm::vec3>(body_count);
		for (unsigned int body_index = 0; body_index < body_count; body_index++)
		{
			file >> simulator->body_positions[frame_index][body_index].x;
			file >> simulator->body_positions[frame_index][body_index].y;
			file >> simulator->body_positions[frame_index][body_index].z;
		}
	}

	
	file.close();

	return simulator;
}
