#include "GranularSubstanceSimulator.h"

#include "RandomGenerator.h"
#include <unordered_set>


using CodeMonkeys::GranularSimulator::GranularSubstanceSimulator;

GranularSubstanceSimulator::GranularSubstanceSimulator(unsigned int frame_count, float timestep_size, unsigned int particle_count, float particle_size,  float particle_mass, float kd, float kr, float alpha, float beta, float mu) :
	frame_count(frame_count),
	particle_count(particle_count),
	particle_size(particle_size),
	timestep_size(timestep_size),
	particle_mass(particle_mass),
	collision_detector(VoxelCollisionDetector(particle_size * 2.0f)),
	kd(kd),
	kr(kr),
	alpha(alpha),
	beta(beta),
	mu(mu),
	Fg(particle_mass * glm::vec3(0.0f, -9.8f, 0.0f))
{
	this->initial_particle_positions = new glm::vec3[particle_count];
	this->initial_particle_velocities = new glm::vec3[particle_count];

	float spacing_multiplier = 4.0f;
	float particle_start_height = 0.5f;	// meters
	float velocity = 0.5f;					// meter/second
	int columns = 5;
	for (unsigned int j = 0; j < (int)glm::ceil(this->particle_count / (float)columns); j++)
	{
		for (unsigned int i = 0; i < columns; ++i)
		{
			float x = i * particle_size * spacing_multiplier - (columns * particle_size * spacing_multiplier * 0.5f) + RandomGenerator::RandomBetween(0.0f, particle_size);
			int index = i + j * columns;

			this->initial_particle_positions[index] = glm::vec3(x, particle_start_height + j * spacing_multiplier * particle_size, 0.0f);
			this->initial_particle_velocities[index] = glm::vec3(0, 0, 0);
			//this->initial_particle_velocities[i] = RandomGenerator::RandomVecBetween(-velocity, velocity, 0, velocity, 0, 0);
			this->initial_particle_velocities[i] = RandomGenerator::RandomVecBetween(-velocity, velocity);
		}
	}
	//this->initial_particle_velocities[0] = glm::vec3(velocity, 0, 0);
	//this->initial_particle_velocities[1] = glm::vec3(-velocity, 0, 0);
}

glm::vec3* GranularSubstanceSimulator::get_particle_positions_at(unsigned int t)
{
	return this->particle_positions_simulation[t];
}

void GranularSubstanceSimulator::init_simulation(unsigned int timesteps)
{
	this->particle_positions_simulation = new glm::vec3*[timesteps];
	this->particle_positions_simulation[0] = new glm::vec3[this->particle_count];

	this->particle_velocities_simulation = new glm::vec3*[timesteps];
	this->particle_velocities_simulation[0] = new glm::vec3[this->particle_count];

	for (unsigned int i = 0; i < this->particle_count; ++i)
	{
		this->particle_positions_simulation[0][i] = glm::vec3(this->initial_particle_positions[i]);
		this->particle_velocities_simulation[0][i] = glm::vec3(this->initial_particle_velocities[i]);
		this->collision_detector.insert(i, this->particle_positions_simulation[0][i]);
	}
}

void GranularSubstanceSimulator::generate_simulation()
{
	init_simulation(this->frame_count);
	
	for (unsigned int i = 1; i < this->frame_count; i++)
	{
		this->generate_timestep(i, this->timestep_size);
	}
}

glm::vec3 GranularSubstanceSimulator::calculate_contact_force(glm::vec3 this_position, glm::vec3 this_velocity, glm::vec3 other_position, glm::vec3 other_velocity)
{
	glm::vec3 contact_force = glm::vec3();
	glm::vec3 offset = other_position - this_position;
	float offset_length = glm::length(offset);
	float overlap = this->particle_size * 2 - offset_length; // E = max( 0, R1 + R2 - ||x2 - x1||)
	if (overlap > 0.0f)
	{
		glm::vec3 normal = offset / offset_length; // N = (x2 - x1) / (||x2 - x1||)

		glm::vec3 relative_velocity =  this_velocity - other_velocity; // V = v1 - v2
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

void GranularSubstanceSimulator::generate_timestep(unsigned int timestep, float dt)
{
	this->particle_positions_simulation[timestep] = new glm::vec3[this->particle_count];
	this->particle_velocities_simulation[timestep] = new glm::vec3[this->particle_count];
	for (unsigned int i = 0; i < this->particle_count; ++i)
	{
		glm::vec3 this_position = this->particle_positions_simulation[timestep - 1][i];
		glm::vec3 this_velocity = this->particle_velocities_simulation[timestep - 1][i];

		std::unordered_set<unsigned int> possible_collisions = this->collision_detector.get_indices_in_voxel(this_position);

		glm::vec3 total_force(0.0f);
		for (int other_index : possible_collisions)
		{
			if (other_index != i) // We don't want a particle to collide with itself...
			{
				glm::vec3 other_position = this->particle_positions_simulation[timestep - 1][other_index];
				glm::vec3 other_velocity = this->particle_velocities_simulation[timestep - 1][other_index];
				total_force += this->calculate_contact_force(this_position, this_velocity, other_position, other_velocity);
			}
		}

		// ground contact force
		total_force += this->calculate_contact_force(this_position, this_velocity, glm::vec3(this_position.x, -this->particle_size, this_position.z), glm::vec3());
		// left contact force
		total_force += this->calculate_contact_force(this_position, this_velocity, glm::vec3(-0.5f - this->particle_size, this_position.y, this_position.z), glm::vec3());
		// right contact force
		total_force += this->calculate_contact_force(this_position, this_velocity, glm::vec3(0.5f + this->particle_size, this_position.y, this_position.z), glm::vec3());
		// front contact force
		total_force += this->calculate_contact_force(this_position, this_velocity, glm::vec3(this_position.x, this_position.y, 0.1f + this->particle_size), glm::vec3());
		// back contact force
		total_force += this->calculate_contact_force(this_position, this_velocity, glm::vec3(this_position.x, this_position.y, -0.1f - this->particle_size), glm::vec3());

		total_force += Fg;
		
		glm::vec3 new_this_velocity = this->particle_velocities_simulation[timestep - 1][i] + total_force / this->particle_mass * dt;

		glm::vec3 new_this_position = this->particle_positions_simulation[timestep - 1][i] + new_this_velocity * dt;

		//if (new_this_position.y < this->particle_size)
		//{
		//	new_this_position.y = this->particle_size;
		//	new_this_velocity.y = 0;
		//}

		this->particle_positions_simulation[timestep][i] = new_this_position;
		this->particle_velocities_simulation[timestep][i] = new_this_velocity;
	}

	for (unsigned int i = 0; i < this->particle_count; ++i)
	{
		this->collision_detector.update(i, this->particle_positions_simulation[timestep - 1][i], this->particle_positions_simulation[timestep][i]);
	}
}

