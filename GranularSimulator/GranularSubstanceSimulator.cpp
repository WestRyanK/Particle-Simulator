#include "GranularSubstanceSimulator.h"

#include "GranularSimulationLoader.h"
#include "BodyParticleGenerator.h"
#include "RandomGenerator.h"
#include <iostream>
#include <unordered_set>


using CodeMonkeys::GranularSimulator::GranularSubstanceSimulator;

GranularSubstanceSimulator::GranularSubstanceSimulator(unsigned int frame_count, float timestep_size, float max_particle_size, float particle_mass, float kd, float kr, float alpha, float beta, float mu) :
	frame_count(frame_count),
	max_particle_size(max_particle_size),
	timestep_size(timestep_size),
	particle_mass(particle_mass),
	collision_detector(VoxelCollisionDetector(max_particle_size * 2.0f)),
	kd(kd),
	kr(kr),
	alpha(alpha),
	beta(beta),
	mu(mu),
	Fg(particle_mass* glm::vec3(0.0f, -9.8f, 0.0f))
{

}

void GranularSubstanceSimulator::init_simulation(std::function<void(GranularSubstanceSimulator*)> setup_simulation)
{
	initial_body_positions = std::vector<glm::vec3>();
	initial_body_rotations = std::vector<glm::mat4>();
	initial_body_velocities = std::vector<glm::vec3>();
	initial_body_angular_velocities = std::vector<glm::vec3>();
	initial_particle_positions = std::vector<glm::vec3>();

	this->is_setting_up_simulation = true;
	setup_simulation(this);
	this->is_setting_up_simulation = false;


	this->body_count = (unsigned int)initial_body_positions.size();
	this->body_positions = std::vector<std::vector<glm::vec3>>(this->frame_count, std::vector<glm::vec3>(this->body_count, glm::vec3()));
	this->body_rotations = std::vector<std::vector<glm::mat4>>(this->frame_count, std::vector<glm::mat4>(this->body_count, glm::mat4()));
	this->body_angular_velocities = std::vector<std::vector<glm::vec3>>(this->frame_count, std::vector<glm::vec3>(this->body_count, glm::vec3()));
	this->body_velocities = std::vector<std::vector<glm::vec3>>(this->frame_count, std::vector<glm::vec3>(this->body_count, glm::vec3()));

	this->body_positions[0] = initial_body_positions;
	this->body_rotations[0] = initial_body_rotations;
	this->body_angular_velocities[0] = initial_body_angular_velocities;
	this->body_velocities[0] = initial_body_velocities;


	this->particle_count = (unsigned int)initial_particle_positions.size();
	this->particle_positions_simulation = std::vector<std::vector<glm::vec3>>(this->frame_count, std::vector<glm::vec3>(this->particle_count, glm::vec3()));
	this->particle_positions_simulation[0] = initial_particle_positions;
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

	for (unsigned int j = 0; j < body_offsets.size(); j++)
	{
		this->particle_body_indices.push_back(body_index);
		this->body_particle_indices[body_index].insert(this->particle_body_indices.size() - 1);
	}

	this->initial_body_positions.push_back(body_position);
	this->initial_body_rotations.push_back(glm::mat4(1.0f));
	this->initial_body_angular_velocities.push_back(glm::vec3(0.0f));
	this->initial_body_velocities.push_back(body_velocity);

	unsigned int i = 0;
	for (body_particle_index_it it = this->body_particle_indices[body_index].begin(); it != this->body_particle_indices[body_index].end(); it++)
	{
		glm::vec3 position = body_position + this->body_offsets[body_index][i];

		this->initial_particle_positions.push_back(position);
		this->particle_sizes.push_back(body_particle_sizes[i]);
		this->collision_detector.insert(*it, position);
		i++;
	}
}

const std::vector<glm::vec3>& GranularSubstanceSimulator::get_body_positions_at(unsigned int t) const
{
	return this->body_positions[t];
}

const std::vector<glm::vec3>& GranularSubstanceSimulator::get_particle_positions_at(unsigned int t) const
{
	return this->particle_positions_simulation[t];
}

const std::vector<glm::mat4>& GranularSubstanceSimulator::get_particle_rotations_at(unsigned int t) const
{
	//glm::mat4* particle_rotations = new glm::mat4[this->particle_count];
	//for (unsigned int i = 0; i < this->particle_count; i++)
	//{
	//	particle_rotations[i] = this->body_rotations[t][this->particle_body_indices[i]];
	//}

	//return particle_rotations;
	return std::vector<glm::mat4>();
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
	for (unsigned int i = 1; i < this->frame_count; i++)
	{
		this->generate_timestep(i, this->timestep_size);
	}
}

void GranularSubstanceSimulator::calculate_contact_force_and_torque(float this_particle_size, glm::vec3 this_particle_position, glm::vec3 this_particle_velocity, glm::vec3 this_body_angular_velocity, glm::vec3 this_body_position, float other_particle_size, glm::vec3 other_particle_position, glm::vec3 other_particle_velocity, glm::vec3 other_body_angular_velocity, glm::vec3& out_force, glm::vec3& out_torque)
{
	glm::vec3 offset = other_particle_position - this_particle_position;
	glm::vec3 contact_offset = offset * 0.5f;
	glm::vec3 this_contact_velocity = this_particle_velocity + glm::cross(this_body_angular_velocity, contact_offset);
	glm::vec3 other_contact_velocity = other_particle_velocity + glm::cross(other_body_angular_velocity, -contact_offset);

	out_force = this->calculate_contact_force(this_particle_size, other_particle_size, offset, this_contact_velocity, other_contact_velocity);
	out_torque = glm::cross((contact_offset + this_particle_position) - this_body_position, out_force);
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

void GranularSubstanceSimulator::generate_timestep(unsigned int current_frame, float dt)
{
	std::cout << "Simulating frame " << current_frame << " of " << this->frame_count << std::endl;

	glm::vec3 contact_force, contact_torque;
	for (unsigned int this_body_index = 0; this_body_index < this->body_count; this_body_index++)
	{
		glm::vec3 total_body_force(0.0f);
		glm::vec3 total_body_torque(0.0f);

		body_particle_index body_particles = this->body_particle_indices[this_body_index];
		glm::vec3 this_body_position = this->body_positions[current_frame - 1][this_body_index];
		glm::vec3 this_body_velocity = this->body_velocities[this->previous_state_index][this_body_index];
		glm::vec3 this_body_angular_velocity = this->body_angular_velocities[this->previous_state_index][this_body_index];

		for (body_particle_index_it it = body_particles.begin(); it != body_particles.end(); it++)
		{
			unsigned int this_particle_index = *it;
			glm::vec3 this_particle_position = this->particle_positions_simulation[current_frame - 1][this_particle_index];
			float this_particle_size = this->particle_sizes[this_particle_index];

			std::unordered_set<unsigned int> possible_collisions = this->collision_detector.get_indices_in_voxel(this_particle_position);

			for (int other_particle_index : possible_collisions)
			{
				unsigned int other_body_index = this->particle_body_indices[other_particle_index];
				if (other_body_index != this_body_index) // We don't want a particle to collide with another particle in its own rigid body...
				{
					float other_particle_size = this->particle_sizes[other_particle_index];
					glm::vec3 other_particle_position = this->particle_positions_simulation[current_frame - 1][other_particle_index];
					glm::vec3 other_body_velocity = this->body_velocities[this->previous_state_index][other_body_index];
					glm::vec3 other_body_angular_velocity = this->body_angular_velocities[this->previous_state_index][other_body_index];

					this->calculate_contact_force_and_torque(this_particle_size, this_particle_position, this_body_velocity, this_body_angular_velocity, this_body_position, other_particle_size, other_particle_position, other_body_velocity, other_body_angular_velocity, contact_force, contact_torque);

					total_body_force += contact_force;
					total_body_torque += contact_torque;
				}
			}

			// ground contact force
			this->calculate_contact_force_and_torque(this_particle_size, this_particle_position, this_body_velocity, this_body_angular_velocity, this_body_position, this_particle_size, glm::vec3(this_particle_position.x, -this_particle_size, this_particle_position.z), glm::vec3(), glm::vec3(), contact_force, contact_torque);
			total_body_force += contact_force;
			total_body_torque += contact_torque;
			// left contact force
			this->calculate_contact_force_and_torque(this_particle_size, this_particle_position, this_body_velocity, this_body_angular_velocity, this_body_position, this_particle_size, glm::vec3(-1.0f - this_particle_size, this_particle_position.y, this_particle_position.z), glm::vec3(), glm::vec3(), contact_force, contact_torque);
			//total_body_force += contact_force;
			//total_body_torque += contact_torque;
			// right contact force
			this->calculate_contact_force_and_torque(this_particle_size, this_particle_position, this_body_velocity, this_body_angular_velocity, this_body_position, this_particle_size, glm::vec3(1.0f + this_particle_size, this_particle_position.y, this_particle_position.z), glm::vec3(), glm::vec3(), contact_force, contact_torque);
			//total_body_force += contact_force;
			//total_body_torque += contact_torque;
			// front contact force
			this->calculate_contact_force_and_torque(this_particle_size, this_particle_position, this_body_velocity, this_body_angular_velocity, this_body_position, this_particle_size, glm::vec3(this_particle_position.x, this_particle_position.y, 1.0f + this_particle_size), glm::vec3(), glm::vec3(), contact_force, contact_torque);
			//total_body_force += contact_force;
			//total_body_torque += contact_torque;
			// back contact force
			this->calculate_contact_force_and_torque(this_particle_size, this_particle_position, this_body_velocity, this_body_angular_velocity, this_body_position, this_particle_size, glm::vec3(this_particle_position.x, this_particle_position.y, -1.0f - this_particle_size), glm::vec3(), glm::vec3(), contact_force, contact_torque);
			//total_body_force += contact_force;
			//total_body_torque += contact_torque;

			total_body_force += Fg;
		}

		glm::vec3 new_this_body_velocity = this->body_velocities[this->previous_state_index][this_body_index] + total_body_force / this->particle_mass * dt; // a = F/m       v' = v + a * dt
		glm::vec3 new_this_body_position = this->body_positions[current_frame - 1][this_body_index] + new_this_body_velocity * dt;   // p' = p + v * dt

		glm::vec3 new_this_body_angular_velocity = this->body_angular_velocities[this->previous_state_index][this_body_index] + total_body_torque / this->particle_mass * dt; // a = T/I      // w' = w + a * dt
		glm::mat4 new_this_body_rotation = this->body_rotations[current_frame - 1][this_body_index];
		float new_this_body_angular_velocity_magnitude = glm::length(new_this_body_angular_velocity);
		if (new_this_body_angular_velocity_magnitude > 0.0f)
		{
			new_this_body_rotation = glm::rotate(new_this_body_rotation, new_this_body_angular_velocity_magnitude, new_this_body_angular_velocity);  // theta
		}


		this->body_rotations[current_frame][this_body_index] = new_this_body_rotation;
		this->body_positions[current_frame][this_body_index] = new_this_body_position;
		this->body_velocities[this->current_state_index][this_body_index] = new_this_body_velocity;

		unsigned int i = 0;
		for (body_particle_index_it it = body_particles.begin(); it != body_particles.end(); it++)
		{
			this->particle_positions_simulation[current_frame][*it] = this->body_positions[current_frame][this_body_index] + glm::vec3(this->body_rotations[current_frame][this_body_index] * glm::vec4(this->body_offsets[this_body_index][i], 0.0f));
			i++;
		}
	}

	for (unsigned int i = 0; i < this->particle_count; ++i)
	{
		this->collision_detector.update(i, this->particle_positions_simulation[current_frame - 1][i], this->particle_positions_simulation[current_frame][i]);
	}

	if (current_frame >= this->frame_count - 1)
	{
		std::cout << "\a";
		GranularSimulationLoader::save_simulation("simulation.sim", this);
	}

	std::swap(this->current_state_index, this->previous_state_index);
}

