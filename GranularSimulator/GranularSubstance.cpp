#include "GranularSubstance.h"

#include "../Engine/Assets/ColorMaterial.h"
#include "../Engine/Assets/Model3D.h"
#include "../Engine/Assets/ModelLoader.h"
#include "../Engine/Objects/Billboard.h"
#include "BodyParticleGenerator.h"
#include "GranularSimulationLoader.h"
#include "RandomGenerator.h"
#include <iostream>

using CodeMonkeys::GranularSimulator::GranularSubstance;
using namespace CodeMonkeys::Engine::Assets;
using namespace CodeMonkeys::Engine::Objects;


GranularSubstance::GranularSubstance(Model3D* model, unsigned int frame_count, float timestep_size, float particle_size, float particle_mass, ShaderProgram* shader) :
	InstancedObject3D(model, "granular_substance_simulator", 0),
	frame_count(frame_count),
	particle_size(particle_size),
	timestep_size(timestep_size),
	play_speed(1.0f),
	current_time(0.0f),
	current_frame(0),
	finished_simulating(false)
{
	this->simulator = new GranularSubstanceSimulator(
		this->frame_count,		//	frame_count		(#)
		this->timestep_size,	//	timestep_size	(second)
		particle_mass,			//	particle_mass	(kg)
		200.0f,					//	kd				(??)
		100000.0f,				//	kr				(??)
		1.0f/2.0f,				//	alpha			(#)
		3.0f/2.0f,				//	beta			(#)
		0.3f);					//	mu				(#)

	bool load_saved_simulation = false;
	if (load_saved_simulation)
	{
		this->simulator = GranularSimulationLoader::load_simuation("simulation.sim");
	}
	else
	{
		this->particle_count = 1000;

		this->simulator->init_simulation([&](GranularSubstanceSimulator* simulator)
			{
				float spacing_multiplier = 5.0f;
				float max_velocity = 0.5f;					// meter/second 
				int columns = 10;
				float spacing = this->particle_size * spacing_multiplier;
				float offset_distance = (columns - 1) * spacing * 0.5f;
				float start_height = 0.5f;


				for (unsigned int body_index = 0; body_index < this->particle_count; ++body_index)
				{
					unsigned int ix = body_index % columns;
					unsigned int iz = (body_index / columns) % columns;
					unsigned int iy = (body_index / (columns * columns));

					float x = ix * spacing - offset_distance;
					float z = iz * spacing - offset_distance;
					float y = iy * spacing + start_height;

					std::vector<glm::vec3> body_offsets;
					std::vector<float> body_sizes;
					if (RandomGenerator::RandomBetween(0.0f, 1.0f) < 0.3f)
					{
						body_offsets = BodyParticleGenerator::get_cube_grain_offsets(this->particle_size);
						body_sizes = BodyParticleGenerator::get_cube_grain_sizes(this->particle_size);
					}
					else
					{
						body_offsets = BodyParticleGenerator::get_tetrahedron_grain_offsets(this->particle_size);
						body_sizes = BodyParticleGenerator::get_tetrahedron_grain_sizes(this->particle_size);
					}

					simulator->init_body(
						body_offsets,
						body_sizes,
						glm::vec3(x, y, z), RandomGenerator::RandomVecBetween(-0.5f, 0.5f));
				}
				//simulator->init_body(body_offsets, body_sizes, glm::vec3(0.0f, 0.5f, 0.0f), glm::vec3(2.0f, 0.0f, 0.0f));
			});

		this->simulator->generate_simulation();
	}

	this->spawn_particles(shader);
}

void GranularSubstance::spawn_particles(ShaderProgram* shader)
{
	this->particle_count = this->simulator->get_body_count();
	this->set_instanced_count(this->particle_count);
	this->finished_simulating = true;
}

void GranularSubstance::update(float dt)
{
	if (!this->finished_simulating)
	{
		this->current_frame++;
		this->simulator->generate_timestep(this->current_frame, this->timestep_size);
	}
	else
	{
		this->current_frame = (unsigned int)(this->current_time * this->play_speed / this->timestep_size);
	}

	if (this->current_frame >= this->frame_count)
	{
		this->finished_simulating = true;
		this->current_frame = 0;
		this->current_time = 0;
	}

	std::vector<glm::vec3> particle_positions = this->simulator->get_body_positions_at(this->current_frame);
	//std::vector<float> particle_sizes = this->simulator->get_particle_sizes();
	//glm::mat4* particle_rotations = this->simulator->get_particle_rotations_at(this->current_frame);
	for (unsigned int i = 0; i < this->particle_count; i++)
	{
		glm::mat4 transform(1.0f);
		transform = glm::translate(transform, particle_positions[i]);
		transform = glm::scale(transform, glm::vec3(2.0f * this->particle_size));// particle_sizes[i]));
		this->set_instanced_transform(i, transform);
	}

	//delete particle_rotations;

	this->current_time += dt;
}

void GranularSubstance::control(std::string control_name, float value, float dt)
{
	if (control_name == "play_speed")
	{
		value = 1.0f + value * 0.01f;
		this->play_speed *= value;
		std::cout << "Playspeed: " << this->play_speed << std::endl;
	}
}

void GranularSubstance::control(std::string control_name, int value, float dt)
{
	if (control_name == "reset" && value == 1)
	{
		this->current_frame = 0;
		this->current_time = 0;
	}
}
