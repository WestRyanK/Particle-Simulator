#include "GranularSubstance.h"

#include "../Engine/Assets/ColorMaterial.h"
#include "../Engine/Assets/Model3D.h"
#include "../Engine/Assets/ModelLoader.h"
#include "../Engine/Objects/Billboard.h"
#include "Body.h"
#include "BodyGenerator.h"
#include "GranularSimulationLoader.h"
#include "GranularSubstanceSimulator.h"
#include "Particle.h"
#include "RandomGenerator.h"
#include <iostream>

using CodeMonkeys::GranularSimulator::GranularSubstance;
using namespace CodeMonkeys::GranularSimulator;
using namespace CodeMonkeys::Engine::Assets;
using namespace CodeMonkeys::Engine::Objects;


GranularSubstance::GranularSubstance(Model3D* model, ShaderProgram* shader, bool load_simulation, std::string filepath) :
	InstancedObject3D(model, "granular_substance_simulator", 0),
	play_speed(1.0f),
	current_time(0.0f)
{
	float framerate = 50.0f;			// frame_rate		(frames / second)
	float particle_radius = 0.04f;		// particle_radius	(meters)
	//this->simulation_duration = 150.f / framerate;
	this->simulation_duration = 5.0f;	// simulation_duration	(seconds)
	float particle_density = 2000.f;	//	particle_density	(kg / meter^3)

	this->simulator = new GranularSubstanceSimulator(
		this->simulation_duration,
		framerate,
		1.0f / (framerate * 10.0f),
		particle_density,
		200.0f,							//	kd				(??)
		10000.0f,						//	kr				(??)
		.5f,							//	alpha			(#)
		1.5f,							//	beta			(#)
		0.05f);							//	mu				(#)

	if (load_simulation)
	{
		this->simulator = GranularSimulationLoader::load_simuation(filepath);
		this->simulation_duration = simulator->get_simulation_duration();
	}
	else
	{
		this->plow_scene(particle_radius, particle_density);
		//this->hourglass_scene(particle_radius, particle_density);
		//this->ballsmash_scene(particle_radius, particle_density);
		//this->avalanche_scene(particle_radius, particle_density);
		//this->test_scene(particle_radius, particle_density);

		this->simulator->generate_simulation();
	}

	this->set_instanced_count(this->simulator->get_particle_count());
}

void GranularSubstance::plow_scene(float particle_radius, float particle_density)
{
	this->simulator->init_simulation([&](GranularSubstanceSimulator* simulator)
		{
			this->spawn_particles(simulator, glm::vec3(-.75f, particle_radius, -.75f), glm::vec3(.75f, particle_radius * 10, .75f), particle_radius, particle_density);

			this->spawn_floor(simulator, 2.f, particle_radius, particle_density);

			Body body;
			std::vector<Particle> particles;
			BodyGenerator::generate_plane(particle_radius, particle_density, glm::vec3(-.2f, particle_radius, -.2f), glm::vec3(.2f, particle_radius, -.2f), glm::vec3(-.2f, particle_radius * 8, -.2f), body, particles);
			body.is_movable = false;
			simulator->add_body(body, particles, glm::vec3(0.f, particle_radius * 2, -2.f), glm::vec3(0.f, 0.f, 1.f));
		});
}

// 161.4 seconds
// 0.645 seconds per frame
void GranularSubstance::avalanche_scene(float particle_radius, float particle_density)
{
	this->simulator->init_simulation([&](GranularSubstanceSimulator* simulator)
		{
			this->spawn_particles(simulator, glm::vec3(-1.3f, 1.2f, -1.f), glm::vec3(-.8f, 1.8f, 1.f), particle_radius, particle_density);

			this->spawn_floor(simulator, 2.f, 0.5f * particle_radius, particle_density);

			Body body;
			std::vector<Particle> particles;
			glm::vec3 position = BodyGenerator::generate_plane(0.5f * particle_radius, particle_density, glm::vec3(6.f, .5f, 1.f), glm::vec3(6.f, .5f, -1.f), glm::vec3(7.f, 1.1f, 1.f), body, particles);
			//position.x = -position.x;
			body.is_movable = false;
			simulator->add_body(body, particles, position);
		});
}

void GranularSubstance::ballsmash_scene(float particle_radius, float particle_density)
{
	this->simulator->init_simulation([&](GranularSubstanceSimulator* simulator)
		{
			this->spawn_particles(simulator, glm::vec3(-.5f, particle_radius * 2, -.5f), glm::vec3(.5f, particle_radius * 15, .5f), particle_radius, particle_density);
			this->spawn_floor(simulator, 2.f, particle_radius, particle_density);

			Body body;
			std::vector<Particle> particles;
			//BodyGenerator::generate_sphere_grain(particle_radius * 3.f, 20.f * particle_density, body, particles);
			//simulator->add_body(body, particles, glm::vec3(-1.5f, .2f, -1.5f), glm::vec3(2.5f, 3.f, 2.5f));

			particles.clear();
			position = BodyGenerator::generate_plane(particle_radius, particle_density, glm::vec3(-.5f, particle_radius, .6f), glm::vec3(-.1f, particle_radius, .6), glm::vec3(-.5f, particle_radius * 4, .6f), body, particles);
			simulator->add_body(body, particles, position);

		});
}

// 937 seconds to render
// .937 seconds per frame
void GranularSubstance::hourglass_scene(float particle_radius, float particle_density)
{
	this->simulator->init_simulation([&](GranularSubstanceSimulator* simulator)
		{
			this->spawn_particles(simulator, glm::vec3(-.4f, .8f, -.4f), glm::vec3(.4f, 1.5f, .4f), particle_radius, particle_density);
			this->spawn_floor(simulator, 2.f, particle_radius, particle_density);

			float hole_radius = .15f;
			float hourglass_top = 1.f;
			float hourglass_bottom = .5f;
			float hourglass_max_radius = .8f;

			Body body;
			std::vector<Particle> particles;
			glm::vec3 position = BodyGenerator::generate_plane(particle_radius, particle_density, glm::vec3(-hole_radius, hourglass_bottom, -hourglass_max_radius), glm::vec3(-hole_radius, hourglass_bottom, hourglass_max_radius), glm::vec3(-hourglass_max_radius, hourglass_top, -hourglass_max_radius), body, particles);
			position.x = -position.x;
			body.is_movable = false;
			body.is_visible = false;
			simulator->add_body(body, particles, position);

			particles.clear();
			position = BodyGenerator::generate_plane(particle_radius, particle_density, glm::vec3(hole_radius, hourglass_bottom, -hourglass_max_radius), glm::vec3(hole_radius, hourglass_bottom, hourglass_max_radius), glm::vec3(hourglass_max_radius, hourglass_top, -hourglass_max_radius), body, particles);
			position.x = -position.x;
			body.is_movable = false;
			body.is_visible = false;
			simulator->add_body(body, particles, position);

			particles.clear();
			position = BodyGenerator::generate_plane(particle_radius, particle_density, glm::vec3(-hourglass_max_radius, hourglass_bottom, hole_radius), glm::vec3(hourglass_max_radius, hourglass_bottom, hole_radius), glm::vec3(-hourglass_max_radius, hourglass_top, hourglass_max_radius), body, particles);
			position.z = -position.z;
			body.is_movable = false;
			body.is_visible = false;
			simulator->add_body(body, particles, position);

			particles.clear();
			position = BodyGenerator::generate_plane(particle_radius, particle_density, glm::vec3(-hourglass_max_radius, hourglass_bottom, -hole_radius), glm::vec3(hourglass_max_radius, hourglass_bottom, -hole_radius), glm::vec3(-hourglass_max_radius, hourglass_top, -hourglass_max_radius), body, particles);
			position.z = -position.z;
			body.is_movable = false;
			body.is_visible = false;
			simulator->add_body(body, particles, position);
		});
}

void GranularSubstance::spawn_particles(GranularSubstanceSimulator* simulator, glm::vec3 min, glm::vec3 max, float particle_radius, float particle_density)
{
	float spacing_multiplier = 2.5f;
	float spacing = particle_radius * spacing_multiplier;

	glm::vec3 size = max - min;
	glm::ivec3 counts = (glm::ivec3)(size / spacing);


	for (unsigned int y = 0; y < counts.y; y++)
	{
		for (unsigned int z = 0; z < counts.z; z++)
		{
			for (unsigned int x = 0; x < counts.x; x++)
			{
				glm::vec3 position = glm::vec3(x * spacing, y * spacing, z * spacing) + min;
				glm::vec3 velocity = RandomGenerator::RandomVecBetween(-.5f, .5f);

				Body body;
				std::vector<Particle> particles;
				if (RandomGenerator::RandomBetween(0.f, 1.f) < .5f)
				{
					BodyGenerator::generate_cube_grain(particle_radius, particle_density, body, particles);
				}
				else
				{
					BodyGenerator::generate_tetrahedron_grain(particle_radius, particle_density, body, particles);
				}

				simulator->add_body(body, particles, position, velocity);
			}
		}
	}
}

void GranularSubstance::spawn_floor(GranularSubstanceSimulator* simulator, float floor_size, float particle_radius, float particle_density)
{
	float floor_half_size = floor_size * .5f;
	Body body;
	std::vector<Particle> particles;
	position = BodyGenerator::generate_plane(particle_radius * 1.f, particle_density, glm::vec3(-floor_half_size, -particle_radius, -floor_half_size), glm::vec3(floor_half_size, -particle_radius, -floor_half_size), glm::vec3(-floor_half_size, -particle_radius, floor_half_size), body, particles);
	body.is_movable = false;
	simulator->add_body(body, particles, position);
}

void GranularSubstance::test_scene(float particle_radius, float particle_density)
{
	this->simulator->init_simulation([&](GranularSubstanceSimulator* simulator)
		{
			Body body;
			std::vector<Particle> particles;
			BodyGenerator::generate_cube_grain(particle_radius, particle_density, body, particles);

			simulator->add_body(body, particles, glm::vec3(0.f, 4.f * particle_radius, 0.f), glm::vec3(0.f));

			this->spawn_floor(simulator, .5f, particle_radius, particle_density);
		});
}

void GranularSubstance::update(float dt)
{
	if (this->current_time > this->simulation_duration)
	{
		this->current_time = 0;
	}

	State state = this->simulator->get_simulation_state_at(this->current_time);
	std::vector<Particle> particles = this->simulator->get_particles();
	std::vector<Body> bodies = this->simulator->get_bodies();
	for (unsigned int i = 0; i < state.particle_positions.size(); i++)
	{
		glm::mat4 transform(1.0f);
		if (bodies[particles[i].body_index].is_visible)
		{
			transform = glm::translate(transform, state.particle_positions[i]);
			transform = glm::scale(transform, glm::vec3(2.0f * particles[i].radius));
		}
		else
		{
			transform = glm::mat4(0.f);
		}
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
