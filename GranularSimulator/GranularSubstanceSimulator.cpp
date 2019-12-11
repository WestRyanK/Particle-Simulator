#include "GranularSubstanceSimulator.h"

#include "BodyParticleGenerator.h"
#include "GranularSimulationLoader.h"
#include "RandomGenerator.h"
#include "State.h"
#include <algorithm>
#include <iostream>
#include <unordered_set>


using CodeMonkeys::GranularSimulator::GranularSubstanceSimulator;

using namespace CodeMonkeys::GranularSimulator;


GranularSubstanceSimulator::GranularSubstanceSimulator(unsigned int frame_count, float timestep_size, float particle_mass, float kd, float kr, float alpha, float beta, float mu) :
	frame_count(frame_count),
	timestep_size(timestep_size),
	particle_mass(particle_mass),
	kd(kd),
	kr(kr),
	alpha(alpha),
	beta(beta),
	mu(mu),
	Fg(particle_mass* glm::vec3(0.0f, -9.8f, 0.0f)),
	initial_state(State(0, 0))
{
}

void GranularSubstanceSimulator::init_simulation(std::function<void(GranularSubstanceSimulator*)> setup_simulation)
{
	this->initial_state = State(0, 0);

	this->is_setting_up_simulation = true;
	setup_simulation(this);
	this->is_setting_up_simulation = false;


	this->body_count = (unsigned int)this->initial_state.body_positions.size();
	this->particle_count = (unsigned int)this->initial_state.particle_positions.size();
	this->states = std::vector<State>(this->frame_count, State(this->particle_count, this->body_count));

	this->states[0] = this->initial_state;

	float max_particle_size = *std::max_element(this->particle_sizes.begin(), this->particle_sizes.end());

	if (this->collision_detector != nullptr)
	{
		delete this->collision_detector;
	}
	this->collision_detector = new VoxelCollisionDetector(max_particle_size * 2.0f);
	//for (unsigned int i = 0; i < this->initial_particle_positions.size(); i++)
	//{
	//	this->collision_detector->insert(i, this->initial_particle_positions[i]);
	//}
}

void GranularSubstanceSimulator::init_body(std::vector<glm::vec3> body_offsets, std::vector<float> body_particle_sizes, glm::vec3 body_position, glm::vec3 body_velocity)
{
	if (!this->is_setting_up_simulation)
	{
		throw "init_body() can only be called from the lambda passed to init_simulation!";
	}

	unsigned int body_index = (unsigned int)this->body_particle_indices.size();
	this->body_particle_indices.push_back(std::set<int>());

	this->body_offsets.push_back(body_offsets);

	for (unsigned int i = 0; i < body_offsets.size(); i++)
	{
		this->particle_body_indices.push_back(body_index);
		this->body_particle_indices[body_index].insert(this->particle_body_indices.size() - 1);

		this->particle_sizes.push_back(body_particle_sizes[i]);

		this->initial_state.particle_positions.push_back(body_offsets[i] + body_position);
	}

	this->initial_state.body_positions.push_back(body_position);
	this->initial_state.body_rotations.push_back(glm::mat4(1.0f));
	this->initial_state.body_angular_velocities.push_back(glm::vec3(0.0f));
	this->initial_state.body_velocities.push_back(body_velocity);
}

const State& GranularSubstanceSimulator::get_simulation_state_at(unsigned int t) const
{
	return this->states[t];
}

const std::vector<float>& GranularSubstanceSimulator::get_particle_sizes() const
{
	return this->particle_sizes;
}

unsigned int GranularSubstanceSimulator::get_body_count()
{
	return this->body_count;
}

unsigned int GranularSubstanceSimulator::get_particle_count()
{
	return this->particle_count;
}

void GranularSubstanceSimulator::generate_simulation()
{
	float dt = this->timestep_size;
	for (unsigned int i = 1; i < this->frame_count; i++)
	{
		dt = this->generate_timestep(i, dt);
	}
}

void GranularSubstanceSimulator::calculate_contact_force_and_torque(float this_particle_size, glm::vec3 this_particle_position, glm::vec3 this_particle_velocity, glm::vec3 this_body_angular_velocity, glm::vec3 this_body_position, float other_particle_size, glm::vec3 other_particle_position, glm::vec3 other_particle_velocity, glm::vec3 other_body_angular_velocity, glm::vec3& out_force, glm::vec3& out_torque)
{
	glm::vec3 offset = other_particle_position - this_particle_position;
	glm::vec3 offset_normal = glm::normalize(offset);
	glm::vec3 this_contact_offset = offset - offset_normal * other_particle_size;
	glm::vec3 other_contact_offset = offset - offset_normal * this_particle_size;
	//glm::vec3 this_contact_offset = offset * 0.5f;
	//glm::vec3 other_contact_offset = this_contact_offset;

	glm::vec3 this_contact_velocity = this_particle_velocity + glm::cross(this_body_angular_velocity, this_contact_offset);
	glm::vec3 other_contact_velocity = other_particle_velocity + glm::cross(other_body_angular_velocity, -other_contact_offset);

	out_force = this->calculate_contact_force(this_particle_size, other_particle_size, offset, this_contact_velocity, other_contact_velocity);
	out_torque = glm::cross((this_contact_offset + this_particle_position) - this_body_position, out_force);
}

glm::vec3 GranularSubstanceSimulator::calculate_contact_force(float this_particle_size, float other_particle_size, glm::vec3 offset_this_to_other, glm::vec3 this_velocity, glm::vec3 other_velocity)
{
	glm::vec3 contact_force = glm::vec3();
	float offset_length = glm::length(offset_this_to_other);
	float overlap = this_particle_size + other_particle_size - offset_length; // E = max( 0, R1 + R2 - ||x2 - x1||)
	if (overlap > 0.0f)
	{
		glm::vec3 normal = offset_this_to_other / offset_length; // N = (x2 - x1) / (||x2 - x1||)

		glm::vec3 relative_velocity = this_velocity - other_velocity; // V = v1 - v2
		float normal_velocity = glm::dot(relative_velocity, normal); // E. = V . N
		glm::vec3 tangent_velocity = relative_velocity - normal_velocity * normal; // Vt = V - E. * N
		float tangent_velocity_magnitude = glm::length(tangent_velocity); // vt = ||Vt||

		//float normal_force_magnitude = (kd * normal_velocity + kr * overlap); // fn + kd * E. + kr * E = 0
		float normal_force_magnitude = (kd * glm::pow(overlap, alpha) * normal_velocity + kr * glm::pow(overlap, beta)); // fn + kd * E^alpha * E. + kr * E ^ beta = 0
		glm::vec3 normal_force = -normal_force_magnitude * normal;

		if (tangent_velocity_magnitude > 0.0f)
		{
			//glm::vec3 shear_force = -glm::min(mu * normal_force_magnitude, this->kr * tangent_velocity_magnitude) * tangent_velocity / tangent_velocity_magnitude;
			glm::vec3 shear_force = -mu * normal_force_magnitude * tangent_velocity / tangent_velocity_magnitude;
			contact_force += shear_force;
		}

		contact_force += normal_force;
	}

	return contact_force;
}

void GranularSubstanceSimulator::calculate_all_contact_force_and_torque(unsigned int this_body_index, const State& state, glm::vec3& total_body_force, glm::vec3& total_body_torque)
{
	glm::vec3 contact_force;
	glm::vec3 contact_torque;
	total_body_force = glm::vec3(0.0f);
	total_body_torque = glm::vec3(0.0f);

	body_particle_index body_particles = this->body_particle_indices[this_body_index];
	glm::vec3 this_body_position = state.body_positions[this_body_index];
	glm::vec3 this_body_velocity = state.body_velocities[this_body_index];
	glm::vec3 this_body_angular_velocity = state.body_angular_velocities[this_body_index];

	for (body_particle_index_it it = body_particles.begin(); it != body_particles.end(); it++)
	{
		unsigned int this_particle_index = *it;
		glm::vec3 this_particle_position = state.particle_positions[this_particle_index];
		float this_particle_size = this->particle_sizes[this_particle_index];

		std::unordered_set<unsigned int> possible_collisions = this->collision_detector->get_indices_in_voxel(this_particle_position);

		for (int other_particle_index : possible_collisions)
		{
			unsigned int other_body_index = this->particle_body_indices[other_particle_index];
			if (other_body_index != this_body_index) // We don't want a particle to collide with another particle in its own rigid body...
			{
				float other_particle_size = this->particle_sizes[other_particle_index];
				glm::vec3 other_particle_position = state.particle_positions[other_particle_index];
				glm::vec3 other_body_velocity = state.body_velocities[other_body_index];
				glm::vec3 other_body_angular_velocity = state.body_angular_velocities[other_body_index];

				this->calculate_contact_force_and_torque(this_particle_size, this_particle_position, this_body_velocity, this_body_angular_velocity, this_body_position, other_particle_size, other_particle_position, other_body_velocity, other_body_angular_velocity, contact_force, contact_torque);

				total_body_force += contact_force;
				total_body_torque += contact_torque;
			}
		}

		float wall_spacing = 1.0f;
		// ground contact force
		this->calculate_contact_force_and_torque(this_particle_size, this_particle_position, this_body_velocity, this_body_angular_velocity, this_body_position, this_particle_size, glm::vec3(this_particle_position.x, -this_particle_size, this_particle_position.z), glm::vec3(), glm::vec3(), contact_force, contact_torque);
		total_body_force += contact_force;
		total_body_torque += contact_torque;
		// left contact force
		this->calculate_contact_force_and_torque(this_particle_size, this_particle_position, this_body_velocity, this_body_angular_velocity, this_body_position, this_particle_size, glm::vec3(-wall_spacing - this_particle_size, this_particle_position.y, this_particle_position.z), glm::vec3(), glm::vec3(), contact_force, contact_torque);
		total_body_force += contact_force;
		total_body_torque += contact_torque;
		// right contact force
		this->calculate_contact_force_and_torque(this_particle_size, this_particle_position, this_body_velocity, this_body_angular_velocity, this_body_position, this_particle_size, glm::vec3(wall_spacing + this_particle_size, this_particle_position.y, this_particle_position.z), glm::vec3(), glm::vec3(), contact_force, contact_torque);
		total_body_force += contact_force;
		total_body_torque += contact_torque;
		// front contact force
		this->calculate_contact_force_and_torque(this_particle_size, this_particle_position, this_body_velocity, this_body_angular_velocity, this_body_position, this_particle_size, glm::vec3(this_particle_position.x, this_particle_position.y, wall_spacing + this_particle_size), glm::vec3(), glm::vec3(), contact_force, contact_torque);
		total_body_force += contact_force;
		total_body_torque += contact_torque;
		// back contact force
		this->calculate_contact_force_and_torque(this_particle_size, this_particle_position, this_body_velocity, this_body_angular_velocity, this_body_position, this_particle_size, glm::vec3(this_particle_position.x, this_particle_position.y, -wall_spacing - this_particle_size), glm::vec3(), glm::vec3(), contact_force, contact_torque);
		total_body_force += contact_force;
		total_body_torque += contact_torque;

		total_body_force += Fg;
	}
}

void GranularSubstanceSimulator::calculate_all_body_accelerations(State state, float t, std::vector<glm::vec3>& body_accelerations, std::vector<glm::vec3>& body_angular_accelerations)
{
	for (unsigned int i = 0; i < this->particle_count; ++i)
	{
		this->collision_detector->update(i, state.particle_positions[i]);
	}

	for (unsigned int this_body_index = 0; this_body_index < this->body_count; this_body_index++)
	{
		glm::vec3 total_body_force;
		glm::vec3 total_body_torque;

		this->calculate_all_contact_force_and_torque(this_body_index, state, total_body_force, total_body_torque);

		float total_body_mass = this->particle_mass * this->body_particle_indices[this_body_index].size();
		body_accelerations[this_body_index] = total_body_force / total_body_mass;
		body_angular_accelerations[this_body_index] = total_body_torque / this->particle_mass;
	}
}

void GranularSubstanceSimulator::evaluate(const State& input_state, float dt, const StateDerivative& input_derivative, StateDerivative& output_derivative)
{
	State output_state = input_state + (input_derivative * dt);
	output_state.t = input_state.t + dt;
	output_state.update_particle_positions(this->body_particle_indices, this->body_offsets);

	output_derivative.body_positions_dt = output_state.body_velocities;
	output_derivative.body_rotations_dt = output_state.body_angular_velocities;
	this->calculate_all_body_accelerations(output_state, input_state.t + dt, output_derivative.body_velocities_dt, output_derivative.body_angular_velocities_dt);
}

void GranularSubstanceSimulator::integrate_rk4(const State& input_state, float dt, State& output_state)
{
	StateDerivative k1(this->body_count), k2(k1), k3(k1), k4(k1);

	evaluate(input_state, 0.0f, StateDerivative(this->body_count), k1);
	evaluate(input_state, dt * 0.5f, k1, k2);
	evaluate(input_state, dt * 0.5f, k2, k3);
	evaluate(input_state, dt, k3, k4);

	output_state = input_state + ((k1 + (k2 + k3) * 2.0f + k4) * (1.0f / 6.0f)) * dt;
	output_state.update_particle_positions(this->body_particle_indices, this->body_offsets);

	output_state.t = input_state.t + dt;
}

float GranularSubstanceSimulator::integrate_rkf45(const State& input_state, float dt, State& output_state)
{
	bool correct_timestep_size = false;
	const float TOLERANCE = 0.001f;
	while (!correct_timestep_size)
	{
		std::cout << "dt: " << dt << std::endl;
		StateDerivative k1(this->body_count), k2(k1), k3(k1), k4(k1), k5(k1), k6(k1);

		evaluate(input_state, 0.0f, StateDerivative(this->body_count), k1);
		evaluate(input_state, (1.0f / 4.0f) * dt, (k1 * (1.0f / 4.0f)) * dt, k2);
		evaluate(input_state, (3.0f / 8.0f) * dt, (k1 * (3.0f / 32.0f) + k2 * (9.0f / 32.0f)) * dt, k3);
		evaluate(input_state, (12.0f / 13.0f) * dt, (k1 * (1932.0f / 2197.0f) - k2 * (7200.0f / 2197.0f) + k3 * (7296.0f / 2197.0f)) * dt, k4);
		evaluate(input_state, (1.0f) * dt, (k1 * (439.0f / 216.0f) - k2 * (8.0f) + k3 * (3680.0f / 513.0f) - k4 * (845.0f / 4104.0f)) * dt, k5);
		evaluate(input_state, (1.0f / 2.0f) * dt, (k1 * (-8.0f / 27.0f) + k2 * (2.0f) - k3 * (3544.0f / 2565.0f) + k4 * (1859.0f / 4104.0f) - k5 * (11.0f / 40.0f)) * dt, k6);

		State yt = input_state + (k1 * (25.0f / 216.0f) + k3 * (1408.0f / 2565.0f) + k4 * (2197.0f / 4101.0f) - k5 * (1.0f / 5.0f)) * dt;

		State zt = input_state + (k1 * (16.0f / 135.0f) + k3 * (6656.0f / 12825.0f) + k4 * (28561.0f / 56430.0f) - k5 * (9.0f / 50.0f) + k6 * (2.0f / 55.0f)) * dt;

		float max_dzy = 0.0f;
		std::vector<glm::vec3> body_positions_dt = (zt - yt).body_positions_dt;
		for (unsigned int i = 0; i < body_positions_dt.size(); i++)
		{
			float length = glm::length(body_positions_dt[i]);
			if (length > max_dzy)
			{
				max_dzy = length;
			}
		}

		float s = glm::pow(TOLERANCE * dt / (2.0f * glm::abs(max_dzy)), 0.25f);
		std::cout << "S: " << s << std::endl;
		if (s < 0.75f && dt > this->min_timestep_size)
		{
			dt = glm::max(dt * 0.5f, this->min_timestep_size);
		}
		else
		{
			correct_timestep_size = true;
			yt.update_particle_positions(this->body_particle_indices, this->body_offsets);
			output_state = yt;
		}


		
		if (s > 1.5f)
		{
			dt = glm::min(dt * 2.0f, this->max_timestep_size);
		}
	}

	return dt;
}

void GranularSubstanceSimulator::integrate_euler(const State& input_state, float dt, State& output_state)
{
	//output_state.t = input_state.t + dt;

	//StateDerivative derivative = StateDerivative(this->body_count);

	//this->calculate_all_body_accelerations(input_state, input_state.t + dt, derivative.body_velocities_dt, derivative.body_angular_velocities_dt);

	//for (unsigned int i = 0; i < this->body_count; i++)
	//{
	//	output_state.body_velocities[i] = input_state.body_velocities[i] + derivative.body_velocities_dt[i] * dt;
	//	output_state.body_angular_velocities[i] = input_state.body_angular_velocities[i] + derivative.body_angular_velocities_dt[i] * dt;
	//}

	//derivative.body_positions_dt = output_state.body_velocities;
	//derivative.body_rotations_dt = output_state.body_angular_velocities;

	//for (unsigned int i = 0; i < this->body_count; i++)
	//{
	//	output_state.body_positions[i] = input_state.body_positions[i] + derivative.body_positions_dt[i] * dt;
	//	output_state.body_rotations[i] = this->rotate(input_state.body_rotations[i], derivative.body_rotations_dt[i] * dt);
	//}

	//output_state.update_particle_positions(this->body_particle_indices, this->body_offsets);
}

float GranularSubstanceSimulator::generate_timestep(unsigned int current_frame, float dt)
{
	std::cout << "Simulating frame  " << current_frame << " of " << this->frame_count << std::endl;

	//this->integrate_euler(this->states[current_frame - 1], dt, this->states[current_frame]);
	//this->integrate_rk4(this->states[current_frame - 1], dt, this->states[current_frame]);
	float next_timestep_size = this->integrate_rkf45(this->states[current_frame - 1], dt, this->states[current_frame]);

	if (current_frame >= this->frame_count - 1)
	{
		std::cout << "\a";
		GranularSimulationLoader::save_simulation("simulations/simulation.sim", this);
	}

	return next_timestep_size;
}

