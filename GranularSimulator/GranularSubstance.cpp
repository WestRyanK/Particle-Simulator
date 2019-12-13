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


GranularSubstance::GranularSubstance(Model3D* model, ShaderProgram* shader) :
	InstancedObject3D(model, "granular_substance_simulator", 0),
	play_speed(1.0f),
	current_time(0.0f)
{
	float framerate = 50.0f;			// frame_rate		(frames / second)
	float particle_size = 0.04f;		// particle_size	(meters)
	this->simulation_duration = 10.0f;	// simulation_duration	(seconds)

	this->simulator = new GranularSubstanceSimulator(
		this->simulation_duration,
		framerate,
		1.0f / (framerate * 10.0f),
		2.f,							//	particle_mass_to_size_ratio	(kg / meters)
		200.0f,							//	kd				(??)
		100000.0f,						//	kr				(??)
		1.0f / 2.0f,					//	alpha			(#)
		3.0f / 2.0f,					//	beta			(#)
		0.2f);							//	mu				(#)

	bool load_saved_simulation = false;
	if (load_saved_simulation)
	{
		this->simulator = GranularSimulationLoader::load_simuation("simulations/simulation.sim");
	}
	else
	{
		this->simulator->init_simulation([&](GranularSubstanceSimulator* simulator)
			{
				float particle_count = 1;
				float spacing_multiplier = 4.0f;
				float max_velocity = 0.5f;					// meter/second 
				int columns = 2;
				float spacing = particle_size * spacing_multiplier;
				float offset_distance = (columns - 1) * spacing * 0.5f;
				float start_height = 0.5f;


				for (unsigned int body_index = 0; body_index < particle_count; ++body_index)
				{
					unsigned int ix = body_index % columns;
					unsigned int iz = (body_index / columns) % columns;
					unsigned int iy = (body_index / (columns * columns));

					float x = ix * spacing - offset_distance;
					float z = iz * spacing - offset_distance;
					float y = iy * spacing + start_height;

					glm::vec3 xyz = glm::vec3(x, y, z);
					glm::vec3 velocity = glm::vec3(.5f, .5f, .5f);
					//glm::vec3 velocity =  RandomGenerator::RandomVecBetween(-0.5f, 0.5f);
					//if (RandomGenerator::RandomBetween(0.0f, 1.0f) < 0.3f)
					if (true)
					{
						simulator->init_cube_grain(particle_size, xyz, velocity);
					}
					else
					{
						simulator->init_tetrahedron_grain(particle_size, xyz, velocity);
					}
				}

				// Flat
				simulator->init_plane(particle_size * .25f, glm::vec3(-1.f, -.2f, -1.f), glm::vec3(1.f, -.2f, -1.f), glm::vec3(-1.f, -.2f, 1.f));

				// Slope
				//simulator->init_plane(particle_size * .25f, glm::vec3(-1.f, -.2f, -1.f), glm::vec3(1.f, -.2f, -1.f), glm::vec3(-1.f, .5f, 1.f));
				//simulator->init_plane(particle_size * .5f, glm::vec3(-1.f, -.2f, -1.f), glm::vec3(1.f, -.2f, -1.f), glm::vec3(-1.f, -.2f, -1.3f));

				//simulator->init_plane(.025f, glm::vec3(0.f, .3f, 0.f), glm::vec3(.1f, .3f, 0.f), glm::vec3(0.f, .6f, 0.f), true);
				//simulator->init_plane(.025f, glm::vec3(0.f, .3f, .1f), glm::vec3(.1f, .3f, .1f), glm::vec3(0.f, .9f, .3f), true);
				//simulator->init_plane(.025f, glm::vec3(.1f, .3f, .1f), glm::vec3(.1f, .3f, .3f), glm::vec3(.1f, .6f, .1f), true);

				//std::vector<glm::vec3> body_offsets = BodyParticleGenerator::get_cube_grain_offsets(particle_size);
				//std::vector<float> body_sizes = BodyParticleGenerator::get_cube_grain_sizes(particle_size);
				//simulator->init_body(body_offsets, body_sizes, glm::vec3(0.0f, 0.08f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f));
			});

		this->simulator->generate_simulation();
	}

	this->spawn_particles(shader);
}

void GranularSubstance::spawn_particles(ShaderProgram* shader)
{
	this->set_instanced_count(this->simulator->get_particle_count());
}

void GranularSubstance::update(float dt)
{
	if (this->current_time > this->simulation_duration)
	{
		this->current_time = 0;
	}

	State state = this->simulator->get_simulation_state_at(this->current_time);
	std::vector<float> particle_sizes = this->simulator->get_particle_sizes();
	for (unsigned int i = 0; i < state.particle_positions.size(); i++)
	{
		glm::mat4 transform(1.0f);
		transform = glm::translate(transform, state.particle_positions[i]);
		transform = glm::scale(transform, glm::vec3(2.0f * particle_sizes[i]));
		this->set_instanced_transform(i, transform);
	}

	this->current_time += dt * this->play_speed;
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
		this->current_time = 0;
	}
}
