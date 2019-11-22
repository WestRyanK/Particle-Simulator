#include "../Collision/BoundingSphere.h"

using CodeMonkeys::Engine::Collision::BoundingSphere;


BoundingSphere::BoundingSphere(vec3 center, float radius)
{
	this->center = center;
	this->radius = radius;
}

vec3 BoundingSphere::get_center()
{
	return this->center;
}

void BoundingSphere::set_center(vec3 center)
{
	this->center = center;
}

float BoundingSphere::get_radius()
{
	return this->radius;
}

void BoundingSphere::set_radius(float radius)
{
	this->radius = radius;
}