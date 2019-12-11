#ifndef _STATE_H_
#define _STATE_H_

#include <glm/glm.hpp>
#include <set>
#include <vector>

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

		std::vector<glm::mat4> body_rotations;
		std::vector<glm::vec3> body_angular_velocities;

		State(unsigned int particle_count, unsigned int body_count);
		void update_particle_positions(const std::vector<std::set<int>>& body_particle_indices, const std::vector<std::vector<glm::vec3>>& body_offsets);
		glm::mat4 rotate(glm::mat4 rotation_matrix, glm::vec3 rotation) const;
	};

	struct StateDerivative
	{
		std::vector<glm::vec3> body_positions_dt;
		std::vector<glm::vec3> body_velocities_dt;

		std::vector<glm::vec3> body_rotations_dt;
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
