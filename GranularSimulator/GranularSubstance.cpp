#include "GranularSubstance.h"

#include "../Engine/Assets/ColorMaterial.h"
#include "../Engine/Assets/Model3D.h"
#include "../Engine/Assets/ModelLoader.h"
#include "../Engine/Objects/Billboard.h"
#include "RandomGenerator.h"
#include <iostream>

using CodeMonkeys::GranularSimulator::GranularSubstance;
using namespace CodeMonkeys::Engine::Assets;
using namespace CodeMonkeys::Engine::Objects;


GranularSubstance::GranularSubstance(unsigned int frame_count, float timestep_size, unsigned int particle_count, float particle_size, float particle_mass, ShaderProgram* shader) :
	Object3D(nullptr, "granular_substance_simulator"),
	frame_count(frame_count),
	particle_count(particle_count),
	particle_size(particle_size),
	particles(nullptr),
	timestep_size(timestep_size),
	play_speed(1.0f),
	current_time(0.0f),
	current_frame(0),
	finished_simulating(false)
{
	this->simulator = new GranularSubstanceSimulator(
		this->frame_count,		//	frame_count		(#)
		this->timestep_size,	//	timestep_size	(second)
		this->particle_count,	//	particle_count	(#)
		this->particle_size,	//	particle_size	(meter)
		particle_mass,			//	particle_mass	(kg)
		200.0f,					//	kd				(??)
		100000.0f,				//	kr				(??)
		1.0f/2.0f,				//	alpha			(#)
		3.0f/2.0f,				//	beta			(#)
		0.1f);					//	mu				(#)

    this->ml_model = new mlModel();
    LoadModel("Assets", "block.obj", *ml_model);

	this->spawn_particles(shader);
}

void GranularSubstance::spawn_particles(ShaderProgram* shader)
{
	this->particles = new Object3D*[this->particle_count];
	for (unsigned int i = 0; i < this->particle_count; i++)
	{
		std::vector<Material*> materials;
		Material* material = new ColorMaterial(shader, false, 20.0f, glm::vec3(1.0f, 1.0f, 1.0f), RandomGenerator::RandomVecBetween(0.0f, 1.0f));
		materials.push_back(material);
		materials.push_back(material);

		Model3D* particle_model = new Model3D(this->ml_model, materials);
		this->particles[i] = new Object3D(particle_model, "particle");
		this->particles[i]->set_scale(2.0f * this->particle_size);
		this->add_child(this->particles[i]);
	}

	this->simulator->init_simulation(this->frame_count);
	this->finished_simulating = false;
}

void GranularSubstance::update(float dt)
{
	if (!this->finished_simulating)
	{
		this->current_frame++;
		std::cout << "Simulating frame " << this->current_frame << " of " << this->frame_count << std::endl;
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

	glm::vec3* particle_positions = this->simulator->get_particle_positions_at(this->current_frame);
	for (unsigned int i = 0; i < this->particle_count; i++)
	{
		this->particles[i]->set_position(particle_positions[i]);
	}

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
