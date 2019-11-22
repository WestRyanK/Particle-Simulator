#pragma once

#include <string>
#include <glm/glm.hpp>
#include "VoxelCollisionDetector.h"

namespace CodeMonkeys::GranularSimulator
{
	class GranularSubstanceSimulator
	{
	public:
		GranularSubstanceSimulator(unsigned int frame_count, float timestep_size, unsigned int particle_count, float particle_size,  float particle_mass, float kd, float kr, float alpha, float beta, float mu);
		void generate_simulation();
		glm::vec3* get_particle_positions_at(unsigned int t);

		void init_simulation(unsigned int timesteps);

		void generate_timestep(unsigned int timestep, float dt);

	private:
		float particle_size;
		float particle_mass;
		float timestep_size;
		unsigned int frame_count;
		unsigned int particle_count;
		float kd;
		float kr;
		float alpha;
		float beta;
		float mu;
		glm::vec3 Fg;


		glm::vec3** particle_positions_simulation = nullptr;
		glm::vec3** particle_velocities_simulation = nullptr;
		glm::vec3* initial_particle_positions = nullptr;
		glm::vec3* initial_particle_velocities = nullptr;

		VoxelCollisionDetector collision_detector;

		glm::vec3 calculate_contact_force(glm::vec3 this_position, glm::vec3 this_velocity, glm::vec3 other_position, glm::vec3 other_velocity);
	};
}
