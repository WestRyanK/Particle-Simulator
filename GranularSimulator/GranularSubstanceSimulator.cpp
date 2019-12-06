#include "GranularSubstanceSimulator.h"

#include "GranularSimulationLoader.h"
#include "BodyParticleGenerator.h"
#include "RandomGenerator.h"
#include <iostream>
#include <algorithm>
#include <unordered_set>


using CodeMonkeys::GranularSimulator::GranularSubstanceSimulator;
using CodeMonkeys::GranularSimulator::State;


State::State(unsigned int particle_count, unsigned int body_count)
{
	this->t = 0.0f;
	this->particle_positions = std::vector<glm::vec3>(particle_count);

	this->body_positions = std::vector<glm::vec3>(body_count);
	this->body_velocities = std::vector<glm::vec3>(body_count);
	this->body_rotations = std::vector<glm::mat4>(body_count, glm::mat4(1.0f));
	this->body_angular_velocities = std::vector<glm::vec3>(body_count);
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


GranularSubstanceSimulator::GranularSubstanceSimulator(unsigned int frame_count, float timestep_size, float particle_mass, float kd, float kr, float alpha, float beta, float mu) :
	frame_count(frame_count),
	timestep_size(timestep_size),
	particle_mass(particle_mass),
	kd(kd),
	kr(kr),
	alpha(alpha),
	beta(beta),
	mu(mu),
	Fg(particle_mass * glm::vec3(0.0f, -9.8f, 0.0f)),
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

unsigned int GranularSubstanceSimulator::get_particle_count()
{
	return this->particle_count;
}

void GranularSubstanceSimulator::generate_simulation()
{
	for (unsigned int i = 1; i < this->frame_count; i++)
	{
		this->generate_timestep(i, this->timestep_size);
	}
}

void GranularSubstanceSimulator::calculate_contact_force_and_torque(float this_particle_size, glm::vec3 this_particle_position, glm::vec3 this_particle_velocity, glm::vec3 this_body_angular_velocity, glm::vec3 this_body_position, float other_particle_size, glm::vec3 other_particle_position, glm::vec3 other_particle_velocity, glm::vec3 other_body_angular_velocity, glm::vec3& out_force, glm::vec3& out_torque)
{
	glm::vec3 offset = other_particle_position - this_particle_position;
	//glm::vec3 this_contact_offset = offset * (this_particle_size / (this_particle_size + other_particle_size));
	//glm::vec3 other_contact_offset = offset - this_contact_offset;
	glm::vec3 this_contact_offset = offset * 0.5f;
	glm::vec3 other_contact_offset = this_contact_offset;

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
		body_angular_accelerations[this_body_index] = total_body_torque / total_body_mass;
	}
}

void GranularSubstanceSimulator::evaluate(const State& input_state, float dt, const State& input_derivative, State& output_derivative)
{
	State output_state(this->particle_count, this->body_count);
	output_state.t = input_state.t + dt;
	for (unsigned int i = 0; i < this->body_count; i++)
	{
		output_state.body_positions[i] = input_state.body_positions[i] + input_derivative.body_positions[i] * dt;
		output_state.body_velocities[i] = input_state.body_velocities[i] + input_derivative.body_velocities[i] * dt;

		//output_state.body_rotations[i] = input_state.body_rotations[i] + input_derivative.body_positions[i] * dt;
		//output_state.body_angular_velocities[i] = input_state.body_angular_velocities[i] + input_derivative.body_angular_velocities[i] * dt;
	}
	output_state.update_particle_positions(this->body_particle_indices, this->body_offsets);


	output_derivative.body_positions = output_state.body_velocities;
	this->calculate_all_body_accelerations(output_state, input_state.t + dt, output_derivative.body_velocities, output_derivative.body_angular_velocities);
}

void GranularSubstanceSimulator::integrate_rk4(const State& input_state, float dt, State& output_state)
{
	State k1(this->particle_count, this->body_count), k2(k1), k3(k1), k4(k1);

	evaluate(input_state, 0.0f, State(this->particle_count, this->body_count), k1);
	evaluate(input_state, dt * 0.5f, k1, k2);
	evaluate(input_state, dt * 0.5f, k2, k3);
	evaluate(input_state, dt, k3, k4);

	for (unsigned int this_body_index = 0; this_body_index < this->body_count; this_body_index++)
	{
		glm::vec3 body_positions_dt = 1.0f / 6.0f * (k1.body_positions[this_body_index] + 2.0f * (k2.body_positions[this_body_index] + k3.body_positions[this_body_index]) + k4.body_positions[this_body_index]);
		glm::vec3 body_velocities_dt = 1.0f / 6.0f * (k1.body_velocities[this_body_index] + 2.0f * (k2.body_velocities[this_body_index] + k3.body_velocities[this_body_index]) + k4.body_velocities[this_body_index]);
		output_state.body_positions[this_body_index] = input_state.body_positions[this_body_index] + body_positions_dt * dt;
		output_state.body_velocities[this_body_index] = input_state.body_velocities[this_body_index] + body_velocities_dt * dt;

		//glm::vec3 body_angular_velocities_dt = 1.0f / 6.0f * (k1.body_angular_velocities[i] + 2.0f * (k2.body_angular_velocities[i] + k3.body_angular_velocities[i]) + k4.body_angular_velocities[i]);
		//glm::vec3 dv_dt = 1.0f / 6.0f * (k1.body_velocities[i] + 2.0f * (k2.body_velocities[i] + k3.body_velocities[i]) + k4.body_velocities[i]);
		//state.body_positions[i] += body_positions_dt * dt;
		//state.body_velocities[i] += body_velocities_dt * dt;


		//unsigned int i = 0;
		//for (body_particle_index_it it = this->body_particle_indices[this_body_index].begin(); it != this->body_particle_indices[this_body_index].end(); it++)
		//{
		//	output_state.particle_positions[*it] = input_state.body_positions[this_body_index] + glm::vec3(input_state.body_rotations[this_body_index] * glm::vec4(this->body_offsets[this_body_index][i], 0.0f));
		//	i++;
		//}
	}

	output_state.update_particle_positions(this->body_particle_indices, this->body_offsets);

	output_state.t = input_state.t + dt;
}

void GranularSubstanceSimulator::generate_timestep(unsigned int current_frame, float dt)
{
	std::cout << "Simulating frame " << current_frame << " of " << this->frame_count << std::endl;

	this->integrate_rk4(this->states[current_frame - 1], dt, this->states[current_frame]);

	if (current_frame >= this->frame_count - 1)
	{
		std::cout << "\a";
		GranularSimulationLoader::save_simulation("simulations/simulation.sim", this);
	}

	//std::swap(this->current_state_index, this->previous_state_index);
}

