#include "State.h"

#include "Body.h"
#include "Particle.h"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

using CodeMonkeys::GranularSimulator::State;
using CodeMonkeys::GranularSimulator::StateDerivative;

using namespace CodeMonkeys::GranularSimulator;


StateDerivative::StateDerivative(unsigned int body_count)
{
	this->body_positions_dt = std::vector<glm::vec3>(body_count, glm::vec3(0.f));
	this->body_velocities_dt = std::vector<glm::vec3>(body_count, glm::vec3(0.f));
	this->body_orientations_dt = std::vector<glm::vec3>(body_count, glm::vec3(0.f));
	this->body_angular_velocities_dt = std::vector<glm::vec3>(body_count, glm::vec3(0.f));
}

State::State(unsigned int particle_count, unsigned int body_count)
{
	this->t = 0.0f;
	this->particle_positions = std::vector<glm::vec3>(particle_count);

	this->body_positions = std::vector<glm::vec3>(body_count, glm::vec3(0.f));
	this->body_velocities = std::vector<glm::vec3>(body_count, glm::vec3(0.f));
	this->body_orientations = std::vector<glm::quat>(body_count, glm::quat(0.f, glm::vec3(0.f, 1.f, 0.f)));
	this->body_angular_velocities = std::vector<glm::vec3>(body_count, glm::vec3(0.f));
}

glm::quat State::rotate(glm::quat orientation_quat, glm::vec3 rotation) const
{
	float rotation_magnitude = glm::length(rotation);
	if (rotation_magnitude > 0.0f)
	{
		orientation_quat = glm::rotate(orientation_quat, rotation_magnitude, rotation);
	}

	return orientation_quat;
}

void State::update_particle_positions(const std::vector<Body>& bodies, const std::vector<Particle>& particles)
{
	for (unsigned int this_body_index = 0; this_body_index < this->body_positions.size(); this_body_index++)
	{
		unsigned int i = 0;
		for (body_particle_index_it it = bodies[this_body_index].particle_indices.begin(); it != bodies[this_body_index].particle_indices.end(); it++)
		{
			this->particle_positions[*it] = this->body_positions[this_body_index] + glm::normalize(this->body_orientations[this_body_index]) * particles[*it].offset_from_body;
			i++;
		}
	}
}

State State::interpolate_between_states(const State& state_a, const State& state_b, float time_at_interpolation)
{
	float states_dt = state_b.t - state_a.t;
	float fraction_of_state_a = (time_at_interpolation - state_a.t) / states_dt;
	State interpolated_state = state_a + fraction_of_state_a * (state_b - state_a);

	for (unsigned int i = 0; i < interpolated_state.body_positions.size(); i++)
	{
		interpolated_state.body_orientations[i] = glm::shortMix(state_a.body_orientations[i], state_b.body_orientations[i], fraction_of_state_a);
	}
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
		output.body_orientations_dt[i] = derivative.body_orientations_dt[i] * multiplier;
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
		output.body_orientations_dt[i] = derivative.body_orientations_dt[i] + other.body_orientations_dt[i];
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
		output.body_orientations_dt[i] = derivative.body_orientations_dt[i] - other.body_orientations_dt[i];
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
		output.body_orientations[i] = glm::normalize(state.rotate(state.body_orientations[i], other.body_orientations_dt[i]));
	}
	glm::quat norm = glm::normalize(output.body_orientations[0]);
	float len = glm::length(output.body_orientations[0]);
	float len2 = glm::length(norm);
	glm::vec3 euler = glm::eulerAngles(output.body_orientations[0]);
	glm::vec3 euler2 = glm::eulerAngles(norm);

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
		output.body_orientations_dt[i] = glm::vec3(0.0f);
	}

	return output;
}
