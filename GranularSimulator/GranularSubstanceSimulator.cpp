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


GranularSubstanceSimulator::GranularSubstanceSimulator(float simulation_duration, float framerate, float initial_timestep_size, float particle_mass, float kd, float kr, float alpha, float beta, float mu) :
	simulation_duration(simulation_duration),
	framerate(framerate),
	initial_timestep_size(initial_timestep_size),
	min_timestep_size(1.0f / (framerate * 10)),
	max_timestep_size(1.0f / (framerate * 2)),
	frame_count((unsigned int) glm::ceil(simulation_duration * framerate)),
	particle_mass(particle_mass),
	kd(kd),
	kr(kr),
	alpha(alpha),
	beta(beta),
	mu(mu),
	Fg(particle_mass* glm::vec3(0.0f, -9.8f, 0.0f)),
	current_state(State(0,0)),
	previous_state(State(0,0))
{
}

void GranularSubstanceSimulator::init_simulation(std::function<void(GranularSubstanceSimulator*)> setup_simulation)
{
	this->previous_state = State(0, 0);

	this->is_setting_up_simulation = true;
	setup_simulation(this);
	this->is_setting_up_simulation = false;


	this->body_count = (unsigned int)this->previous_state.body_positions.size();
	this->particle_count = (unsigned int)this->previous_state.particle_positions.size();
	this->frame_states = std::vector<State>(this->frame_count, State(this->particle_count, this->body_count));

	this->previous_state.update_particle_positions(this->body_particle_indices, this->body_offsets);
	this->frame_states[0] = this->previous_state;

	float max_particle_size = *std::max_element(this->particle_sizes.begin(), this->particle_sizes.end());

	if (this->collision_detector != nullptr)
	{
		delete this->collision_detector;
	}
	this->collision_detector = new VoxelCollisionDetector(max_particle_size * 2.0f);
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

		this->previous_state.particle_positions.push_back(body_offsets[i] + body_position);
	}

	this->previous_state.body_positions.push_back(body_position);
	this->previous_state.body_rotations.push_back(glm::mat4(1.0f));
	this->previous_state.body_angular_velocities.push_back(glm::vec3(0.0f));
	this->previous_state.body_velocities.push_back(body_velocity);
}

const State GranularSubstanceSimulator::get_simulation_state_at(float t) const
{
	unsigned int first_frame_index = this->get_frame_index_at_time(t);
	if (first_frame_index < this->frame_count - 1)
	{
		State interpolated_state = State::interpolate_between_states(this->frame_states[first_frame_index], this->frame_states[first_frame_index + 1], t);
		interpolated_state.update_particle_positions(this->body_particle_indices, this->body_offsets);
		return interpolated_state;
	}
	else
	{
		return this->frame_states[this->frame_count - 1];
	}
}

unsigned int GranularSubstanceSimulator::get_frame_index_at_time(float t) const
{
	return t * this->framerate;
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
	const float TOLERANCE = 0.01f;
	while (!correct_timestep_size)
	{
		std::cout << "dt: " << dt << std::endl;
		StateDerivative k1(this->body_count), k2(k1), k3(k1), k4(k1), k5(k1), k6(k1);

		evaluate(input_state, 0.f, StateDerivative(this->body_count), k1);
		evaluate(input_state, (1.f / 4.f) * dt, ((1.f / 4.f) * k1), k2);
		evaluate(input_state, (3.f / 8.f) * dt, ((3.f / 32.f) * k1 + (9.f / 32.f) * k2), k3);
		evaluate(input_state, (12.f / 13.f) * dt, ((1932.f / 2197.f) * k1 - (7200.f / 2197.f) * k2 + (7296.f / 2197.f) * k3), k4);
		evaluate(input_state, (1.f) * dt, ((439.f / 216.f) * k1 - (8.f) * k2 + (3680.f / 513.f) * k3 - (845.f / 4104.f) * k4), k5);
		evaluate(input_state, (1.f / 2.f) * dt, ((-8.f / 27.f) * k1 + (2.f) * k2 - (3544.f / 2565.f) * k3 + (1859.f / 4104.f) * k4 - (11.f / 40.f) * k5), k6);
		//k1 = k1 * dt;
		//k2 = k2 * dt;
		//k3 = k3 * dt;
		//k4 = k4 * dt;
		//k5 = k5 * dt;
		//k6 = k6 * dt;


		State yt = input_state + dt * ((25.f / 216.f) * k1 + (1408.f / 2565.f) * k3 + (2197.f / 4101.f) * k4 - (1.f / 5.f) * k5);
		State zt = input_state + dt * ((16.f / 135.f) * k1 + (6656.f / 12825.f) * k3 + (28561.f / 56430.f) * k4 - (9.f / 50.f) * k5 + (2.f / 55.f) * k6);

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

		//if (s < 1.0f && dt > this->min_timestep_size)
		//{
		//	dt = glm::max(dt * s, this->min_timestep_size);
		//}
		//else
		//{
		//	correct_timestep_size = true;
		//	yt.update_particle_positions(this->body_particle_indices, this->body_offsets);
		//	yt.t = input_state.t + dt;
		//	output_state = yt;

		//	dt = glm::min(dt * s, this->max_timestep_size);
		//}

		if (s < 0.75f && dt > this->min_timestep_size)
		{
			dt = glm::max(dt * 0.5f, this->min_timestep_size);
		}
		else
		{
			correct_timestep_size = true;
			yt.update_particle_positions(this->body_particle_indices, this->body_offsets);
			yt.t = input_state.t + dt;
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

void GranularSubstanceSimulator::generate_simulation()
{
	float dt = this->initial_timestep_size;
	unsigned int frame_to_simulate = 1;
	while (frame_to_simulate < this->frame_count)
	{
		 this->generate_timestep(frame_to_simulate, dt);
	}
}

void GranularSubstanceSimulator::generate_timestep(unsigned int& next_timestep_to_generate, float& dt)
{
	float current_frame_time = next_timestep_to_generate / this->framerate;
	std::cout << "Simulating frame  " << next_timestep_to_generate << " of " << this->frame_count << std::endl;
	std::cout << "Current time: " << this->previous_state.t << std::endl;

	//this->integrate_euler(this->states[current_frame - 1], dt, this->states[current_frame]);
	//this->integrate_rk4(this->states[current_frame - 1], dt, this->states[current_frame]);
	dt = this->integrate_rkf45(this->previous_state, dt, this->current_state);

	if (this->current_state.t > current_frame_time)
	//if (this->previous_state.t <= current_frame_time && this->current_state.t > current_frame_time)
	{
		State frame_state = State::interpolate_between_states(this->previous_state, this->current_state, current_frame_time);
		frame_state.update_particle_positions(this->body_particle_indices, this->body_offsets);
		this->frame_states[next_timestep_to_generate] = frame_state;
		next_timestep_to_generate++;
	}

	if (next_timestep_to_generate >= this->frame_count)
	{
		std::cout << "\a";
		GranularSimulationLoader::save_simulation("simulations/simulation.sim", this);
	}

	this->previous_state = this->current_state;
}

