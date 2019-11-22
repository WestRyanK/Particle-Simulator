#include "BoundingBox.h"

using CodeMonkeys::Engine::Collision::BoundingBox;


BoundingBox::BoundingBox(vec3 corner_max, vec3 corner_min)
{
	throw "Not implemented!";
}

vec3 BoundingBox::get_corner_max()
{
    return this->corner_max;
}

void BoundingBox::set_corner_max(vec3 corner_max)
{
    this->corner_max = corner_max;
}

vec3 BoundingBox::get_corner_min()
{
    return this->corner_min;
}

void BoundingBox::set_corner_min(vec3 corner_min)
{
    this->corner_min = corner_min;
}

float BoundingBox::get_min_x()
{
    return this->corner_min.x;
}

float BoundingBox::get_min_y()
{
    return this->corner_min.y;
}

float BoundingBox::get_min_z()
{
    return this->corner_min.z;
}

float BoundingBox::get_max_x()
{
    return this->corner_max.x;
}

float BoundingBox::get_max_y()
{
    return this->corner_max.y;
}

float BoundingBox::get_max_z()
{
    return this->corner_max.z;
}

void BoundingBox::set_min_x(float x)
{
    this->corner_min.x = x;
}

void BoundingBox::set_min_y(float y)
{
    this->corner_min.y = y;
}

void BoundingBox::set_min_z(float z)
{
    this->corner_min.z = z;
}

void BoundingBox::set_max_x(float x)
{
    this->corner_max.x = x;
}

void BoundingBox::set_max_y(float y)
{
    this->corner_max.y = y;
}

void BoundingBox::set_max_z(float z)
{
    this->corner_max.z = z;
}