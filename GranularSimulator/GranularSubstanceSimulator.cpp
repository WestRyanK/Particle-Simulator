#include "GranularSubstanceSimulator.h"

#include "GranularSimulationLoader.h"
#include "RandomGenerator.h"
#include <iostream>
#include <unordered_set>


using CodeMonkeys::GranularSimulator::GranularSubstanceSimulator;

GranularSubstanceSimulator::GranularSubstanceSimulator(unsigned int frame_count, float timestep_size, unsigned int particle_count, float particle_size, float particle_mass, float kd, float kr, float alpha, float beta, float mu) :
	frame_count(frame_count),
	particle_size(particle_size),
	timestep_size(timestep_size),
	body_count(particle_count),
	particle_mass(particle_mass),
	collision_detector(VoxelCollisionDetector(particle_size * 2.0f)),
	kd(kd),
	kr(kr),
	alpha(alpha),
	beta(beta),
	mu(mu),
	Fg(particle_mass * glm::vec3(0.0f, -9.8f, 0.0f))
{
	this->particle_count = particle_count * PARTICLES_PER_BODY;

	this->particle_positions_simulation = new glm::vec3*[this->frame_count];
	this->particle_positions_simulation[0] = new glm::vec3[this->particle_count];
	this->body_positions = new glm::vec3*[this->frame_count];
	this->body_rotations = new glm::mat4 * [this->frame_count];

	this->body_angular_velocities = new glm::vec3 * [2];
	this->body_velocities = new glm::vec3 * [2];
	for (unsigned int i = 0; i < 2; i++)
	{
		this->body_angular_velocities[i] = new glm::vec3[this->body_count];
		this->body_velocities[i] = new glm::vec3[this->body_count];
	}

	this->particle_body_indices = new int[this->particle_count];
	this->body_particle_indices = new std::set<int>[this->body_count];
	this->body_offsets = new glm::vec3*[this->body_count];
	float offset_scale = 1.0f;

	for (unsigned int body_index = 0; body_index < this->body_count; body_index++)
	{
		this->body_particle_indices[body_index] = std::set<int>();

		this->body_offsets[body_index] = new glm::vec3[PARTICLES_PER_BODY];

		// Single Particles
		//this->body_offsets[body_index][0] = glm::vec3(0.0f, 0.0f, 0.0f) * offset_scale * this->particle_size;

		// Cube
		//this->body_offsets[body_index][0] = glm::vec3(0.0f, 0.0f, 0.0f) * offset_scale * this->particle_size;
		//this->body_offsets[body_index][1] = glm::vec3(1.0f, 1.0f, 1.0f) * offset_scale * this->particle_size;
		//this->body_offsets[body_index][2] = glm::vec3(1.0f, 1.0f, -1.0f) * offset_scale * this->particle_size;
		//this->body_offsets[body_index][3] = glm::vec3(-1.0f, 1.0f, -1.0f) * offset_scale * this->particle_size;
		//this->body_offsets[body_index][4] = glm::vec3(-1.0f, 1.0f, 1.0f) * offset_scale * this->particle_size;
		//this->body_offsets[body_index][5] = glm::vec3(1.0f, -1.0f, 1.0f) * offset_scale * this->particle_size;
		//this->body_offsets[body_index][6] = glm::vec3(1.0f, -1.0f, -1.0f) * offset_scale * this->particle_size;
		//this->body_offsets[body_index][7] = glm::vec3(-1.0f, -1.0f, -1.0f) * offset_scale * this->particle_size;
		//this->body_offsets[body_index][8] = glm::vec3(-1.0f, -1.0f, 1.0f) * offset_scale * this->particle_size;

		// Tetrahedron
		this->body_offsets[body_index][0] = glm::vec3(1.0f, 0.0f, -1.0f / glm::sqrt(2.0f)) * offset_scale * this->particle_size;
		this->body_offsets[body_index][1] = glm::vec3(-1.0f, 0.0f, -1.0f / glm::sqrt(2.0f)) * offset_scale * this->particle_size;
		this->body_offsets[body_index][2] = glm::vec3(0.0f, 1.0f, 1.0f / glm::sqrt(2.0f)) * offset_scale * this->particle_size;
		this->body_offsets[body_index][3] = glm::vec3(0.0f, -1.0f, 1.0f / glm::sqrt(2.0f)) * offset_scale * this->particle_size;

		for (unsigned int j = 0; j < PARTICLES_PER_BODY; j++)
		{
			unsigned int particle_index = body_index * PARTICLES_PER_BODY + j;

			this->body_particle_indices[body_index].insert(particle_index);
			this->particle_body_indices[particle_index] = body_index;
		}
	}
}

void GranularSubstanceSimulator::init_simulation() { 
	this->body_positions[0] = new glm::vec3[this->body_count];
	this->body_rotations[0] = new glm::mat4[this->body_count];

	float spacing_multiplier = 3.0f; 
	float max_velocity = 0.5f;					// meter/second 
	int columns = 10; 
	float spacing = this->particle_size * spacing_multiplier;
	float offset_distance = (columns - 1) * spacing * 0.5f;
	float start_height = 0.5f;


	for (unsigned int body_index = 0; body_index < this->body_count; ++body_index)
	{
		unsigned int ix = body_index % columns;
		unsigned int iz = (body_index / columns) % columns;
		unsigned int iy = (body_index / (columns * columns));

		float x = ix * spacing - offset_distance;
		float z = iz * spacing - offset_distance;
		float y = iy * spacing + start_height;

		this->init_body(body_index, glm::vec3(x, y, z));
	}
}

void GranularSubstanceSimulator::init_body(unsigned int body_index, glm::vec3 body_position)
{
	for (unsigned int i = 0; i < 2; i++)
	{
		this->body_angular_velocities[i][body_index] = glm::vec3(0.0f);
		this->body_velocities[i][body_index] = RandomGenerator::RandomVecBetween(-0.5f, 0.5f);
	}

	this->body_positions[0][body_index] = body_position;
	this->body_rotations[0][body_index] = glm::mat4(1.0f);

	unsigned int i = 0;
	for (body_particle_index_it it = this->body_particle_indices[body_index].begin(); it != this->body_particle_indices[body_index].end(); it++)
	{
		glm::vec3 position = body_position + this->body_offsets[body_index][i];

		this->particle_positions_simulation[0][*it] = position;
		this->collision_detector.insert(*it, position);
		i++;
	}
}

glm::vec3* GranularSubstanceSimulator::get_body_positions_at(unsigned int t)
{
	return this->body_positions[t];
}

glm::vec3* GranularSubstanceSimulator::get_particle_positions_at(unsigned int t)
{
	return this->particle_positions_simulation[t];
}

glm::mat4* GranularSubstanceSimulator::get_particle_rotations_at(unsigned int t)
{
	glm::mat4* particle_rotations = new glm::mat4[this->particle_count];
	for (unsigned int i = 0; i < this->particle_count; i++)
	{
		particle_rotations[i] = this->body_rotations[t][this->particle_body_indices[i]];
	}

	return particle_rotations;
}

void GranularSubstanceSimulator::generate_simulation()
{
	this->init_simulation();

	for (unsigned int i = 1; i < this->frame_count; i++)
	{
		this->generate_timestep(i, this->timestep_size);
	}
}

void GranularSubstanceSimulator::calculate_contact_force_and_torque(glm::vec3 this_particle_position, glm::vec3 this_particle_velocity, glm::vec3 this_body_angular_velocity, glm::vec3 this_body_position, glm::vec3 other_particle_position, glm::vec3 other_particle_velocity, glm::vec3 other_body_angular_velocity, glm::vec3& out_force, glm::vec3& out_torque)
{
	glm::vec3 offset = other_particle_position - this_particle_position;
	glm::vec3 contact_offset = offset * 0.5f;
	glm::vec3 this_contact_velocity = this_particle_velocity + glm::cross(this_body_angular_velocity, contact_offset);
	glm::vec3 other_contact_velocity = other_particle_velocity + glm::cross(other_body_angular_velocity, -contact_offset);

	out_force = this->calculate_contact_force(offset, this_contact_velocity, other_contact_velocity);
	out_torque = glm::cross((contact_offset + this_particle_position) - this_body_position, out_force);
}

glm::vec3 GranularSubstanceSimulator::calculate_contact_force(glm::vec3 offset_this_to_other, glm::vec3 this_velocity, glm::vec3 other_velocity)
{
	glm::vec3 contact_force = glm::vec3();
	float offset_length = glm::length(offset_this_to_other);
	float overlap = this->particle_size * 2 - offset_length; // E = max( 0, R1 + R2 - ||x2 - x1||)
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
			glm::vec3 shear_force = -mu * normal_force_magnitude * tangent_velocity / glm::length(tangent_velocity);
			contact_force += shear_force;
		}

		contact_force += normal_force;
	}

	return contact_force;
}

void GranularSubstanceSimulator::generate_timestep(unsigned int current_frame, float dt)
{
	std::cout << "Simulating frame " << current_frame << " of " << this->frame_count << std::endl;
	this->particle_positions_simulation[current_frame] = new glm::vec3[this->particle_count];
	this->body_positions[current_frame] = new glm::vec3[this->body_count];
	this->body_rotations[current_frame] = new glm::mat4[this->body_count];

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

			std::unordered_set<unsigned int> possible_collisions = this->collision_detector.get_indices_in_voxel(this_particle_position);

			for (int other_particle_index : possible_collisions)
			{
				unsigned int other_body_index = this->particle_body_indices[other_particle_index];
				if (other_body_index != this_body_index) // We don't want a particle to collide with another particle in its own rigid body...
				{
					glm::vec3 other_particle_position = this->particle_positions_simulation[current_frame - 1][other_particle_index];
					glm::vec3 other_body_velocity = this->body_velocities[this->previous_state_index][other_body_index];
					glm::vec3 other_body_angular_velocity = this->body_angular_velocities[this->previous_state_index][other_body_index];

					this->calculate_contact_force_and_torque(this_particle_position, this_body_velocity, this_body_angular_velocity, this_body_position, other_particle_position, other_body_velocity, other_body_angular_velocity, contact_force, contact_torque);

					total_body_force += contact_force;
					total_body_torque += contact_torque;
				}
			}

			// ground contact force
			this->calculate_contact_force_and_torque(this_particle_position, this_body_velocity, this_body_angular_velocity, this_body_position, glm::vec3(this_particle_position.x, -this->particle_size, this_particle_position.z), glm::vec3(), glm::vec3(), contact_force, contact_torque);
			total_body_force += contact_force;
			total_body_torque += contact_torque;
			// left contact force
			this->calculate_contact_force_and_torque(this_particle_position, this_body_velocity, this_body_angular_velocity, this_body_position, glm::vec3(-1.0f - this->particle_size, this_particle_position.y, this_particle_position.z), glm::vec3(), glm::vec3(), contact_force, contact_torque);
			//total_body_force += contact_force;
			//total_body_torque += contact_torque;
			// right contact force
			this->calculate_contact_force_and_torque(this_particle_position, this_body_velocity, this_body_angular_velocity, this_body_position, glm::vec3(1.0f + this->particle_size, this_particle_position.y, this_particle_position.z), glm::vec3(), glm::vec3(), contact_force, contact_torque);
			//total_body_force += contact_force;
			//total_body_torque += contact_torque;
			// front contact force
			this->calculate_contact_force_and_torque(this_particle_position, this_body_velocity, this_body_angular_velocity, this_body_position, glm::vec3(this_particle_position.x, this_particle_position.y, 1.0f + this->particle_size), glm::vec3(), glm::vec3(), contact_force, contact_torque);
			//total_body_force += contact_force;
			//total_body_torque += contact_torque;
			// back contact force
			this->calculate_contact_force_and_torque(this_particle_position, this_body_velocity, this_body_angular_velocity, this_body_position, glm::vec3(this_particle_position.x, this_particle_position.y, -1.0f - this->particle_size), glm::vec3(), glm::vec3(), contact_force, contact_torque);
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
			new_this_body_rotation = glm::rotate(new_this_body_rotation, new_this_body_angular_velocity_magnitude , new_this_body_angular_velocity);  // theta
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

