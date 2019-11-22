#include "../Collision/BoundingMultiSphere.h"

#include "../Collision/BoundingSphere.h"

using CodeMonkeys::Engine::Collision::BoundingMultiSphere;
using CodeMonkeys::Engine::Collision::BoundingSphere;


BoundingMultiSphere::BoundingMultiSphere(vec3 center, vec3 rotation)
{
	this->center = center;
	this->rotation = rotation;
}

void BoundingMultiSphere::set_center(vec3 center)
{
	this->center = center;
}

vec3 BoundingMultiSphere::get_center()
{
	return this->center;
}

void BoundingMultiSphere::set_rotation(vec3 rotation)
{
	this->rotation = rotation;
}

vec3 BoundingMultiSphere::get_rotation()
{
	return this->rotation;
}

set<BoundingSphere*> BoundingMultiSphere::get_spheres()
{
	set<BoundingSphere*> offset_spheres;
	for (auto s : this->spheres)
	{
		vec3 offset = s->get_center();
		mat4 rotate_mat = mat4(1.0);
		rotate_mat = glm::rotate(rotate_mat, radians(this->rotation.y), vec3(0, 1, 0));
		rotate_mat = glm::rotate(rotate_mat, radians(this->rotation.z), vec3(0, 0, 1));
		rotate_mat = glm::rotate(rotate_mat, radians(this->rotation.x), vec3(1, 0, 0));
		offset = vec3(rotate_mat * vec4(offset, 1));

		BoundingSphere* offset_sphere = new BoundingSphere(offset + this->center, s->get_radius());
		offset_spheres.insert(offset_sphere);
	}

	return offset_spheres;
}

void BoundingMultiSphere::free_spheres(set<BoundingSphere*> spheres)
{
	for (auto s : spheres)
	{
		free(s);
	}
}

void BoundingMultiSphere::add_sphere(BoundingSphere* sphere)
{
	this->spheres.insert(sphere);
}