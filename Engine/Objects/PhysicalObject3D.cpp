#include "../Objects/PhysicalObject3D.h"

#include "../Assets/Model3D.h"
#include <string>

using CodeMonkeys::Engine::Objects::PhysicalObject3D;
using namespace CodeMonkeys::Engine::Objects;


PhysicalObject3D::PhysicalObject3D(Model3D* model, std::string name) : Object3D(model, name)
{
    this->velocity = vec3(0.0f, 0.0f, 0.0f);
    this->angular_velocity = vec3(0.0f, 0.0f, 0.0f);
}

vec3 PhysicalObject3D::get_velocity()
{
    return this->velocity;
}

void PhysicalObject3D::set_velocity(vec3 velocity)
{
    this->acceleration = this->velocity - velocity;
    this->velocity = velocity;
}
        
vec3 PhysicalObject3D::get_angular_velocity()
{
    return this->angular_velocity;
}
        
void PhysicalObject3D::set_angular_velocity(vec3 angular_velocity)
{
    this->angular_velocity = angular_velocity;
}

vec3 PhysicalObject3D::get_acceleration()
{
    return this->acceleration;
}

void PhysicalObject3D::update(float dt)
{
    this->acceleration = this->acceleration * 0.90f;
    this->position.x += dt * this->velocity.x;
    this->position.y += dt * this->velocity.y;
    this->position.z += dt * this->velocity.z;

    this->rotation.x += dt * this->angular_velocity.x;
    this->rotation.y += dt * this->angular_velocity.y;
    this->rotation.z += dt * this->angular_velocity.z;
    Object3D::update(dt);
}

void PhysicalObject3D::set_velocity(int axis, float value)
{
    this->velocity[axis] = value;
}