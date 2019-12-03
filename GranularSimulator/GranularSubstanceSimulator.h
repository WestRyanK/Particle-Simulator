#pragma once

#include <string>
#include <glm/glm.hpp>
#include <set>
#include "VoxelCollisionDetector.h"

namespace CodeMonkeys::GranularSimulator
{
	class GranularSimulationLoader;
}

namespace CodeMonkeys::GranularSimulator
{
	typedef std::set<int>::iterator body_particle_index_it;
	typedef std::set<int> body_particle_index;

	class GranularSubstanceSimulator
	{
	public:
		GranularSubstanceSimulator(unsigned int frame_count, float timestep_size, unsigned int particle_count, float particle_size,  float particle_mass, float kd, float kr, float alpha, float beta, float mu);
		void generate_simulation();
		glm::vec3* get_particle_positions_at(unsigned int t);
		glm::vec3* get_body_positions_at(unsigned int t);
		glm::mat4* get_particle_rotations_at(unsigned int t);

		void init_simulation();
		void generate_timestep(unsigned int timestep, float dt);

		friend class GranularSimulationLoader;

	private:
		float particle_size;							// meters
		float particle_mass;							// kg
		float timestep_size;							// seconds
		unsigned int frame_count;						// #   (total_time = frame_count * timestep_size)
		unsigned int particle_count;					// #
		float kd;
		float kr;
		float alpha;
		float beta;
		float mu;										// ratio
		glm::vec3 Fg;									// kg * meters / second

		//const unsigned int PARTICLES_PER_BODY = 1; // Single Particles
		//const unsigned int PARTICLES_PER_BODY = 9; // Cube
		const unsigned int PARTICLES_PER_BODY = 4; // Tetrahedron
		unsigned int body_count;						// #
		glm::mat4** body_rotations = nullptr;
		glm::vec3** body_positions = nullptr;			// meters
		glm::vec3** body_angular_velocities = nullptr;  // radians / second
		glm::vec3** body_velocities = nullptr;			// meters / second
		std::set<int>* body_particle_indices = nullptr;
		int* particle_body_indices = nullptr;
		glm::vec3** body_offsets = nullptr;				// meters
		unsigned int current_state_index = 1;
		unsigned int previous_state_index = 0;

		glm::vec3** particle_positions_simulation = nullptr; // meters

		VoxelCollisionDetector collision_detector;

		void init_body(unsigned int index, glm::vec3 body_center);

		void calculate_contact_force_and_torque(glm::vec3 this_particle_position, glm::vec3 this_particle_velocity, glm::vec3 this_body_angular_velocity, glm::vec3 this_body_position, glm::vec3 other_particle_position, glm::vec3 other_particle_velocity, glm::vec3 other_body_angular_velocity, glm::vec3& out_force, glm::vec3& out_torque);
		glm::vec3 calculate_contact_force(glm::vec3 offset_this_to_other, glm::vec3 this_velocity, glm::vec3 other_velocity);
	};
}
