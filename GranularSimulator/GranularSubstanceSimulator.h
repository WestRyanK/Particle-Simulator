#ifndef _GRANULAR_SUBSTANCE_SIMULATOR_H_
#define _GRANULAR_SUBSTANCE_SIMULATOR_H_

#include "State.h"
#include "VoxelCollisionDetector.h"
#include <functional>
#include <glm/glm.hpp>
#include <set>
#include <string>
#include <vector>

namespace CodeMonkeys::GranularSimulator
{
	class GranularSimulationLoader;
	struct State;
	struct StateDerivative;
}

namespace CodeMonkeys::GranularSimulator
{
	typedef std::set<int>::iterator body_particle_index_it;
	typedef std::set<int> body_particle_index;

	class GranularSubstanceSimulator
	{
	public:
		GranularSubstanceSimulator(float simulation_duration, float framerate, float initial_timestep_size, float particle_mass, float kd, float kr, float alpha, float beta, float mu);
		void generate_simulation();
		const std::vector<float>& get_particle_sizes() const;
		const State get_simulation_state_at(float t) const;

		void init_body(std::vector<glm::vec3> body_offsets, std::vector<float> body_particle_sizes, float total_body_mass, glm::vec3 body_position, glm::mat3 inertial_moment, glm::vec3 body_velocity, bool is_body_movable = true);
		void init_cube_grain(float particle_size, glm::vec3 grain_position, glm::vec3 grain_velocity);
		void init_tetrahedron_grain(float particle_size, glm::vec3 grain_position, glm::vec3 grain_velocity);
		void init_plane(float particle_size, glm::vec3 corner_a, glm::vec3 corner_b, glm::vec3 corner_c, bool is_movable = false);

		void init_simulation(std::function<void(GranularSubstanceSimulator*)> setup_simulation);
		void generate_timestep(unsigned int& next_timestep_to_generate, float& dt);

		unsigned int get_body_count();
		unsigned int get_particle_count();

		friend class GranularSimulationLoader;

	private:
		float particle_mass_to_size_ratio;						// kg / meter
		float initial_timestep_size;							// seconds
		float framerate;										// frames / second
		float simulation_duration;								// seconds
		float max_timestep_size;
		float min_timestep_size;
		unsigned int frame_count;								// #   (total_time = frame_count * timestep_size)
		unsigned int particle_count = 0;						// #
		float kd;
		float kr;
		float alpha;
		float beta;
		float mu;												// ratio
		const glm::vec3 gravity = glm::vec3(0.f, -9.8f, 0.f);	// meters / second^2

		std::vector<State> frame_states;
		State previous_state;
		State current_state;

		unsigned int body_count = 0;									// #
		std::vector<std::vector<glm::vec3>> body_offsets;				// meters
		std::vector<std::set<int>> body_particle_indices;
		std::vector<int> particle_body_indices;
		std::vector<bool> are_bodies_movable;
		std::vector<float> total_body_masses;
		std::vector<glm::mat3> body_inverse_inertial_moments;
		bool is_setting_up_simulation = false;
		std::vector<float> particle_sizes;

		VoxelCollisionDetector* collision_detector = nullptr;


		glm::vec3 calculate_contact_force(float this_particle_size, float other_particle_size, glm::vec3 offset_this_to_other, glm::vec3 this_velocity, glm::vec3 other_velocity);
		void calculate_all_body_accelerations(State state, float t, std::vector<glm::vec3>& body_accelerations, std::vector<glm::vec3>& body_angular_accelerations);
		void calculate_all_contact_force_and_torque(unsigned int this_body_index, const State& state, glm::vec3& total_body_force, glm::vec3& total_body_torque);
		void calculate_contact_force_and_torque(float this_particle_size, glm::vec3 this_particle_position, glm::vec3 this_particle_velocity, glm::vec3 this_body_angular_velocity, glm::vec3 this_body_position, float other_particle_size, glm::vec3 other_particle_position, glm::vec3 other_particle_velocity, glm::vec3 other_body_angular_velocity, glm::vec3& out_force, glm::vec3& out_torque);
		void evaluate(const State& input_state, float dt, const StateDerivative& input_derivative, StateDerivative& output_derivative);
		void integrate_rk4(const State& input_state, float dt, State& output_state);
		float integrate_rkf45(const State& input_state, float dt, State& output_state);
		void integrate_euler(const State& input_state, float dt, State& output_state);

		unsigned int get_frame_index_at_time(float t) const;


	};
}

#endif
