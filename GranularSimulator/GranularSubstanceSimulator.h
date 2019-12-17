#ifndef _GRANULAR_SUBSTANCE_SIMULATOR_H_
#define _GRANULAR_SUBSTANCE_SIMULATOR_H_

#include "State.h"
#include "VoxelCollisionDetector.h"
#include <functional>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <set>
#include <string>
#include <vector>

namespace CodeMonkeys::GranularSimulator
{
	class GranularSimulationLoader;
	struct State;
	struct StateDerivative;
	struct Body;
	struct Particle;
}

namespace CodeMonkeys::GranularSimulator
{
	typedef std::set<int>::iterator body_particle_index_it;
	typedef std::set<int> body_particle_index;

	class GranularSubstanceSimulator
	{
	public:
		GranularSubstanceSimulator(float simulation_duration, float framerate, float initial_timestep_size, float particle_density, float kd, float kr, float alpha, float beta, float mu);

		void add_body(
			Body body, 
			std::vector<Particle> particles,
			glm::vec3 position,
			glm::vec3 velocity = glm::vec3(0.f),
			glm::quat orientation = glm::quat(0.f, glm::vec3(0.f, 1.f, 0.f)),
			glm::vec3 angular_velocity = glm::vec3(0.f));

		void init_simulation(std::function<void(GranularSubstanceSimulator*)> setup_simulation);

		void generate_simulation();
		void generate_timestep(unsigned int& next_timestep_to_generate, float& dt);

		unsigned int get_body_count();
		unsigned int get_particle_count();
		float get_simulation_duration() const;
		const std::vector<Particle>& get_particles() const;
		const std::vector<Body>& get_bodies() const;
		const State get_simulation_state_at(float t) const;

		friend class GranularSimulationLoader;

	private:
		float particle_density;						// kg / meter
		float initial_timestep_size;							// seconds
		float framerate;										// frames / second
		float simulation_duration;								// seconds
		float max_timestep_size;
		float min_timestep_size;
		unsigned int frame_count;								// #   (total_time = frame_count * timestep_size)
		float kd;
		float kr;
		float alpha;
		float beta;
		float mu;												// ratio
		const glm::vec3 gravity = glm::vec3(0.f, -9.8f, 0.f);	// meters / second^2

		std::vector<State> frame_states;
		State previous_state;
		State current_state;

		std::vector<Body> bodies;
		std::vector<Particle> particles;

		bool is_setting_up_simulation = false;
		VoxelCollisionDetector* collision_detector = nullptr;


		glm::vec3 calculate_contact_force(float this_particle_radius, float other_particle_radius, glm::vec3 offset_this_to_other, glm::vec3 this_velocity, glm::vec3 other_velocity);
		void calculate_all_body_accelerations(State state, std::vector<glm::vec3>& body_accelerations, std::vector<glm::vec3>& body_angular_accelerations);
		void calculate_all_contact_force_and_torque(unsigned int this_body_index, const State& state, glm::vec3& total_body_force, glm::vec3& total_body_torque);
		void evaluate(const State& input_state, float dt, const StateDerivative& input_derivative, StateDerivative& output_derivative);
		void integrate_rk4(const State& input_state, float dt, State& output_state);
		float integrate_rkf45(const State& input_state, float dt, State& output_state);
		void integrate_euler(const State& input_state, float dt, State& output_state);
		void integrate_verlet(const State& input_state, float dt, State& output_state);

		unsigned int get_frame_index_at_time(float t) const;
	};
}

#endif
