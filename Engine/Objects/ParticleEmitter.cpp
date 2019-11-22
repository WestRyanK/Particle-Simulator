#include "../Objects/ParticleEmitter.h"

#include "../Objects/Particle.h"

using CodeMonkeys::Engine::Objects::ParticleEmitter;


ParticleEmitter::ParticleEmitter(string name) : 
	PhysicalObject3D(nullptr, name),
	particle_prototype(nullptr)
{
}

void ParticleEmitter::start()
{
    this->is_emitting = true;
}

void ParticleEmitter::stop()
{
    this->is_emitting = false;
}

void ParticleEmitter::cancel()
{
    this->stop();
    for (Object3D* object : this->children)
    {
        Particle* particle = (Particle*)object;
        this->kill_particle(particle);
    }
}

void ParticleEmitter::kill_particle(Particle* particle)
{
    this->remove_child(particle);
}

void ParticleEmitter::update(float dt)
{
    if (this->is_emitting)
    {
        this->emit(dt);
    }
}

void ParticleEmitter::emit(float dt)
{
    Particle* particle = this->particle_prototype->clone();
    this->emit(particle);
}

void ParticleEmitter::emit(Particle* particle_to_emit)
{
    this->add_child(particle_to_emit);
}

void ParticleEmitter::set_particle(Particle* particle_prototype)
{
    this->particle_prototype = particle_prototype;
}