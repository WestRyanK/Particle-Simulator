#pragma once

#include "VoxelCollisionDetector.h"
#include <functional>
#include <glm/glm.hpp>
#include <set>
#include <string>
#include <vector>

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
		GranularSubstanceSimulator(unsigned int frame_count, float timestep_size, float particle_size,  float particle_mass, float kd, float kr, float alpha, float beta, float mu);
		void generate_simulation();
		const std::vector<glm::vec3>& get_particle_positions_at(unsigned int t) const;
		const std::vector<glm::vec3>& get_body_positions_at(unsigned int t) const;
		const std::vector<glm::mat4>& get_particle_rotations_at(unsigned int t) const;
		const std::vector<float>& get_particle_sizes() const;

		void init_body(std::vector<glm::vec3> body_offsets, std::vector<float> body_particle_sizes, glm::vec3 body_position, glm::vec3 body_velocity);
		void init_simulation(std::function<void(GranularSubstanceSimulator*)> setup_simulation);
		void generate_timestep(unsigned int timestep, float dt);

		unsigned int get_body_count();
		unsigned int get_particle_count();

		friend class GranularSimulationLoader;

	private:
		float max_particle_size;							// meters
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

		unsigned int body_count;						// #
		std::vector<std::vector<glm::mat4>> body_rotations;
		std::vector<std::vector<glm::vec3>> body_positions;			// meters
		std::vector<std::vector<glm::vec3>> body_angular_velocities;  // radians / second
		std::vector<std::vector<glm::vec3>> body_velocities;			// meters / second
		std::vector<std::vector<glm::vec3>> body_offsets;				// meters
		std::vector<std::set<int>> body_particle_indices;
		std::vector<int> particles_per_body_counts;
		std::vector<int> particle_body_indices;
		unsigned int current_state_index = 1;
		unsigned int previous_state_index = 0;

		bool is_setting_up_simulation = false;
		std::vector<glm::vec3> initial_body_positions;
		std::vector<glm::mat4> initial_body_rotations;
		std::vector<glm::vec3> initial_body_velocities;
		std::vector<glm::vec3> initial_body_angular_velocities;
		std::vector<glm::vec3> initial_particle_positions;

		std::vector<std::vector<glm::vec3>> particle_positions_simulation; // meters
		std::vector<float> particle_sizes;

		VoxelCollisionDetector collision_detector;


		void calculate_contact_force_and_torque(float this_particle_size, glm::vec3 this_particle_position, glm::vec3 this_particle_velocity, glm::vec3 this_body_angular_velocity, glm::vec3 this_body_position, float other_particle_size, glm::vec3 other_particle_position, glm::vec3 other_particle_velocity, glm::vec3 other_body_angular_velocity, glm::vec3& out_force, glm::vec3& out_torque);
		glm::vec3 calculate_contact_force(float this_particle_size, float other_particle_size, glm::vec3 offset_this_to_other, glm::vec3 this_velocity, glm::vec3 other_velocity);
	};
}
