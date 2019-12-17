#include "GranularSubstance.h"

#include "../Engine/Assets/ColorMaterial.h"
#include "../Engine/Assets/Model3D.h"
#include "../Engine/Assets/ModelLoader.h"
#include "../Engine/Objects/Billboard.h"
#include "Body.h"
#include "BodyGenerator.h"
#include "GranularSimulationLoader.h"
#include "Particle.h"
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
	float particle_radius = 0.04f;		// particle_radius	(meters)
	//this->simulation_duration = 150.f / framerate;
	this->simulation_duration = 15.0f;	// simulation_duration	(seconds)
	float particle_density = 1000.f;	//	particle_density	(kg / meter^3)

	this->simulator = new GranularSubstanceSimulator(
		this->simulation_duration,
		framerate,
		1.0f / (framerate * 10.0f),
		particle_density,				
		150.0f,							//	kd				(??)
		10000.0f,						//	kr				(??)
		.5f ,					//	alpha			(#)
		1.5f,					//	beta			(#)
		0.0001f);							//	mu				(#)

	bool load_saved_simulation = false;
	if (load_saved_simulation)
	{
		this->simulator = GranularSimulationLoader::load_simuation("simulations/simulation.sim");
	}
	else
	{
		this->simulator->init_simulation([&](GranularSubstanceSimulator* simulator)
			{
				float particle_count = 10;
				float spacing_multiplier = 4.0f;
				float max_velocity = 0.5f;					// meter/second 
				int columns = 5;
				float spacing = particle_radius * spacing_multiplier;
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

					glm::vec3 position = glm::vec3(x, y, z);
					//glm::vec3 velocity = glm::vec3(.5f, .0f, .5f);
					glm::vec3 velocity =  RandomGenerator::RandomVecBetween(-0.5f, 0.5f);
					Body body;
					std::vector<Particle> particles;
					//if (true)
					if (RandomGenerator::RandomBetween(0.0f, 1.0f) < 0.5f)
					{
						BodyGenerator::generate_cube_grain(particle_radius, particle_density, body, particles);
					}
					else
					{
						BodyGenerator::generate_tetrahedron_grain(particle_radius, particle_density, body, particles);
					}
					simulator->add_body(body, particles, position, velocity);
				}

				//glm::vec3 position = glm::vec3(-.5f, 0.1f, 0.05f);
				Body body;
				std::vector<Particle> particles;
				//BodyGenerator::generate_plane(particle_radius, particle_density, glm::vec3(0.f,.1f,0.f), glm::vec3(2 * particle_radius, 0.1f, 0.f), glm::vec3(0.f, .1f, 4 * particle_radius), body, particles);
				//simulator->add_body(body, particles, position, glm::vec3(.5f,.0f,.0f), glm::quat(glm::radians(45.f), glm::vec3(0.f,1.f,0.f)), glm::vec3(0.f, 3.f,0.f));

				//particles.clear();
				//position = glm::vec3(.5f, 0.1f, 0.f);
				//velocity = glm::vec3(-.5f, .0f, .0f);
				//BodyGenerator::generate_plane(particle_radius, particle_density, glm::vec3(0.f,.1f,0.f), glm::vec3(2 * particle_radius, 0.1f, 0.f), glm::vec3(0.f, .1f, 4 * particle_radius), body, particles);
				//simulator->add_body(body, particles, position, velocity, glm::quat(0.f, glm::vec3(0.f,1.f,0.f)), glm::vec3(0.f, 5.f, 0.f));

				//position = glm::vec3(.5f, 0.1f, 0.f);
				//velocity = glm::vec3(-.5f, .0f, .0f);
				//BodyGenerator::generate_cube_grain(particle_radius, particle_density, body, particles);
				//simulator->add_body(body, particles, position, velocity);

				//// Flat
				particles.clear();
				position = BodyGenerator::generate_plane(particle_radius * 1.5f, particle_density, glm::vec3(-1.f, -.2f, -1.f), glm::vec3(1.f, -.2f, -1.f), glm::vec3(-1.f, -.2f, 1.f), body, particles);
				body.is_movable = false;
				simulator->add_body(body, particles, position);

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
	std::vector<Particle> particles = this->simulator->get_particles();
	for (unsigned int i = 0; i < state.particle_positions.size(); i++)
	{
		glm::mat4 transform(1.0f);
		transform = glm::translate(transform, state.particle_positions[i]);
		transform = glm::scale(transform, glm::vec3(2.0f * particles[i].radius));
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
