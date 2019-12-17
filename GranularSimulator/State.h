#ifndef _STATE_H_
#define _STATE_H_

#include <glm/glm.hpp>
#include <set>
#include <vector>

namespace CodeMonkeys::GranularSimulator
{
	struct Body;
	struct Particle;
}

namespace CodeMonkeys::GranularSimulator
{
	typedef std::set<int>::iterator body_particle_index_it;
	typedef std::set<int> body_particle_index;

	struct State
	{
		float t;

		std::vector<glm::vec3> particle_positions;
		std::vector<glm::vec3> body_positions;
		std::vector<glm::vec3> body_velocities;

		// http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-17-quaternions/
		// http://sjbrown.co.uk/2002/05/01/representing-rotations-in-quaternion-arithmetic/
		std::vector<glm::quat> body_orientations;
		std::vector<glm::vec3> body_angular_velocities;

		State(unsigned int particle_count, unsigned int body_count);
		void update_particle_positions(const std::vector<Body>& bodies, const std::vector<Particle>& particles);
		glm::quat rotate(glm::quat orientation_quat, glm::vec3 rotation) const;
		static State interpolate_between_states(const State& state_a, const State& state_b, float time_at_interpolation);
	};

	struct StateDerivative
	{
		std::vector<glm::vec3> body_positions_dt;
		std::vector<glm::vec3> body_velocities_dt;

		std::vector<glm::vec3> body_orientations_dt;
		std::vector<glm::vec3> body_angular_velocities_dt;

		StateDerivative(unsigned int body_count);
	};

	State operator + (const StateDerivative& other, const State& state);
	State operator + (const State& state, const StateDerivative& other);
	StateDerivative operator - (const State& state, const State& other);
	StateDerivative operator * (const float& multiplier, const StateDerivative& derivative);
	StateDerivative operator * (const StateDerivative& derivative, const float& multiplier);
	StateDerivative operator + (const StateDerivative& derivative, const StateDerivative& other);
	StateDerivative operator - (const StateDerivative& derivative, const StateDerivative& other);
}

#endif
