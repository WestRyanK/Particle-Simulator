#include "../Objects/ParticleObject.h"

#include "../Assets/Model3D.h"
#include "../Collision/BoundingMultiSphere.h"
#include "../Objects/Billboard.h"
#include "../Objects/ParticleEmitter.h"

using CodeMonkeys::Engine::Objects::ParticleObject;
using namespace CodeMonkeys::Engine::Objects;


ParticleObject::ParticleObject(Model3D* model, Billboard* billboard, string name, float total_lifespan, ParticleEmitter* emitter) : PhysicalObject3D(model, name)
{
    this->total_lifespan = total_lifespan;
    this->current_lifespan = 0;
    this->emitter = emitter;
    this->billboard = billboard;
}

ParticleObject::~ParticleObject()
{
    billboard = nullptr;
    emitter = nullptr;
}

ParticleObject* ParticleObject::clone()
{
    Model3D* model_clone = NULL;
    Billboard* billboard_clone = NULL;
    if (this->model != NULL)
        model_clone = this->model->clone();
    if (this->billboard != NULL)
        billboard_clone = this->billboard->clone();

    ParticleObject* particle_clone = new ParticleObject(model_clone, billboard_clone, this->name, this->total_lifespan, this->emitter);
    particle_clone->set_angular_velocity(this->get_angular_velocity());
    particle_clone->set_velocity(this->get_velocity());
    particle_clone->set_position(this->get_position());
    particle_clone->set_rotation(this->get_rotation());
    particle_clone->set_scale(this->get_scale());
    particle_clone->set_collision_region(this->collision_region);

    if (this->billboard != NULL)
    {
        this->billboard->reset();
    }

    return particle_clone;
}

void ParticleObject::update(float dt)
{
    if (this->billboard != NULL)
    {
        this->billboard->update(dt);
    }
    PhysicalObject3D::update(dt);
    if (this->collision_region != NULL)
    {
        this->collision_region->set_center(this->get_position());
    }

    this->current_lifespan += dt;
    if (this->current_lifespan > this->total_lifespan)
    {
        this->emitter->kill_particle(this);
    }
}

void ParticleObject::kill()
{
    emitter->kill_particle(this);
}

ParticleEmitter* ParticleObject::get_emitter()
{
    return this->emitter;
}

void ParticleObject::draw(mat4 total_transform, ShaderProgram* shader)
{
    if (this->billboard != NULL)
    {
        this->billboard->draw(total_transform, shader);
    }

    PhysicalObject3D::draw(total_transform, shader);
}

void ParticleObject::set_collision_region(BoundingMultiSphere* collision_region)
{
    this->collision_region = collision_region;
}

ICollisionRegion* ParticleObject::get_collision_region()
{
    return this->collision_region;
}