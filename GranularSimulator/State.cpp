#include "State.h"

#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>

using CodeMonkeys::GranularSimulator::State;
using CodeMonkeys::GranularSimulator::StateDerivative;

using namespace CodeMonkeys::GranularSimulator;


StateDerivative::StateDerivative(unsigned int body_count)
{
	this->body_positions_dt = std::vector<glm::vec3>(body_count);
	this->body_velocities_dt = std::vector<glm::vec3>(body_count);
	this->body_rotations_dt = std::vector<glm::vec3>(body_count);
	this->body_angular_velocities_dt = std::vector<glm::vec3>(body_count);
}

State::State(unsigned int particle_count, unsigned int body_count)
{
	this->t = 0.0f;
	this->particle_positions = std::vector<glm::vec3>(particle_count);

	this->body_positions = std::vector<glm::vec3>(body_count);
	this->body_velocities = std::vector<glm::vec3>(body_count);
	this->body_rotations = std::vector<glm::mat4>(body_count, glm::mat4(1.0f));
	this->body_angular_velocities = std::vector<glm::vec3>(body_count);
}

glm::mat4 State::rotate(glm::mat4 rotation_matrix, glm::vec3 rotation) const
{
	float rotation_magnitude = glm::length(rotation);
	if (rotation_magnitude > 0.0f)
	{
		rotation_matrix = glm::rotate(rotation_matrix, rotation_magnitude, rotation);
	}

	return rotation_matrix;
}

void State::update_particle_positions(const std::vector<std::set<int>>& body_particle_indices, const std::vector<std::vector<glm::vec3>>& body_offsets)
{
	for (unsigned int this_body_index = 0; this_body_index < this->body_positions.size(); this_body_index++)
	{
		unsigned int i = 0;
		for (body_particle_index_it it = body_particle_indices[this_body_index].begin(); it != body_particle_indices[this_body_index].end(); it++)
		{
			this->particle_positions[*it] = this->body_positions[this_body_index] + glm::vec3(this->body_rotations[this_body_index] * glm::vec4(body_offsets[this_body_index][i], 0.0f));
			i++;
		}
	}
}

State State::interpolate_between_states(const State& state_a, const State& state_b, float time_at_interpolation)
{
	float states_dt = state_b.t - state_a.t;
	float fraction_of_state_a = (time_at_interpolation - state_a.t) / states_dt;
	State interpolated_state = state_a + fraction_of_state_a * (state_b - state_a);
	interpolated_state.t = time_at_interpolation;
	return interpolated_state;
}

StateDerivative CodeMonkeys::GranularSimulator::operator * (const float& multiplier, const StateDerivative& derivative)
{
	return derivative * multiplier;
}

StateDerivative CodeMonkeys::GranularSimulator::operator * (const StateDerivative& derivative, const float& multiplier) 
{
	unsigned int size = derivative.body_positions_dt.size();
	StateDerivative output(size);
	for (unsigned int i = 0; i < size; i++)
	{
		output.body_positions_dt[i] = derivative.body_positions_dt[i] * multiplier;
		output.body_velocities_dt[i] = derivative.body_velocities_dt[i] * multiplier;
		output.body_angular_velocities_dt[i] = derivative.body_angular_velocities_dt[i] * multiplier;
		output.body_rotations_dt[i] = derivative.body_rotations_dt[i] * multiplier;
	}

	return output;
}

StateDerivative CodeMonkeys::GranularSimulator::operator + (const StateDerivative& derivative, const StateDerivative& other) 
{
	if (derivative.body_positions_dt.size() != other.body_positions_dt.size())
	{
		throw "Both derivatives must have same number of bodies";
	}

	unsigned int size = derivative.body_positions_dt.size();
	StateDerivative output(size);
	for (unsigned int i = 0; i < size; i++)
	{
		output.body_positions_dt[i] = derivative.body_positions_dt[i] + other.body_positions_dt[i];
		output.body_velocities_dt[i] = derivative.body_velocities_dt[i] + other.body_velocities_dt[i];
		output.body_angular_velocities_dt[i] = derivative.body_angular_velocities_dt[i] + other.body_angular_velocities_dt[i];
		output.body_rotations_dt[i] = derivative.body_rotations_dt[i] + other.body_rotations_dt[i];
	}

	return output;
}

StateDerivative CodeMonkeys::GranularSimulator::operator - (const StateDerivative& derivative, const StateDerivative& other) 
{
	if (derivative.body_positions_dt.size() != other.body_positions_dt.size())
	{
		throw "Both derivatives must have same number of bodies";
	}

	unsigned int size = derivative.body_positions_dt.size();
	StateDerivative output(size);
	for (unsigned int i = 0; i < size; i++)
	{
		output.body_positions_dt[i] = derivative.body_positions_dt[i] - other.body_positions_dt[i];
		output.body_velocities_dt[i] = derivative.body_velocities_dt[i] - other.body_velocities_dt[i];
		output.body_angular_velocities_dt[i] = derivative.body_angular_velocities_dt[i] - other.body_angular_velocities_dt[i];
		output.body_rotations_dt[i] = derivative.body_rotations_dt[i] - other.body_rotations_dt[i];
	}

	return output;
}


State CodeMonkeys::GranularSimulator::operator + (const StateDerivative& other, const State& state)
{
	return state + other;
}

State CodeMonkeys::GranularSimulator::operator + (const State& state, const StateDerivative& other) 
{
	if (state.body_positions.size() != other.body_positions_dt.size())
	{
		throw "Both derivative and state must have same number of bodies";
	}

	unsigned int body_count = state.body_positions.size();
	State output(state.particle_positions.size(), body_count);
	for (unsigned int i = 0; i < body_count; i++)
	{
		output.body_positions[i] = state.body_positions[i] + other.body_positions_dt[i];
		output.body_velocities[i] = state.body_velocities[i] + other.body_velocities_dt[i];
		output.body_angular_velocities[i] = state.body_angular_velocities[i] + other.body_angular_velocities_dt[i];
		output.body_rotations[i] = state.rotate(state.body_rotations[i], other.body_rotations_dt[i]);
	}

	return output;
}

StateDerivative CodeMonkeys::GranularSimulator::operator - (const State& state, const State& other)
{
	if (state.body_positions.size() != other.body_positions.size())
	{
		throw "Both states must have same number of bodies";
	}

	unsigned int body_count = state.body_positions.size();
	StateDerivative output(body_count);
	for (unsigned int i = 0; i < body_count; i++)
	{
		output.body_positions_dt[i] = state.body_positions[i] - other.body_positions[i];
		output.body_velocities_dt[i] = state.body_velocities[i] - other.body_velocities[i];
		output.body_angular_velocities_dt[i] = state.body_angular_velocities[i] - other.body_angular_velocities[i];
		output.body_rotations_dt[i] = glm::vec3(0.0f);
		//output.body_rotations_dt[i] = state.body_rotations[i] - other.body_rotations[i];
	}

	return output;
}
