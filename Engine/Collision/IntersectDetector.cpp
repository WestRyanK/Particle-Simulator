#include "../Collision/IntersectDetector.h"

#include "../Collision/BoundingMultiSphere.h"
#include "../Collision/BoundingSphere.h"
#include "../Collision/ICollisionRegion.h"

using CodeMonkeys::Engine::Collision::IntersectDetector;
using namespace CodeMonkeys::Engine::Collision;


bool IntersectDetector::is_intersection(ICollisionRegion* region_a, ICollisionRegion* region_b)
{
	BoundingSphere* sphere_a = dynamic_cast<BoundingSphere*> (region_a);
	BoundingSphere* sphere_b = dynamic_cast<BoundingSphere*> (region_b);

	BoundingMultiSphere* multi_sphere_a = dynamic_cast<BoundingMultiSphere*> (region_a);
	BoundingMultiSphere* multi_sphere_b = dynamic_cast<BoundingMultiSphere*> (region_b);

	if (sphere_a != NULL && sphere_b != NULL)
		return is_intersection((BoundingSphere*)sphere_a, (BoundingSphere*)sphere_b);
	else if (multi_sphere_a != NULL && sphere_b != NULL)
	{
		auto spheres = multi_sphere_a->get_spheres();
		bool intersect = false;
		for (auto s : spheres)
		{
			intersect |= is_intersection((BoundingSphere*)s, (BoundingSphere*)sphere_b);
		}
		multi_sphere_a->free_spheres(spheres);
		return intersect;
	}
	else if (multi_sphere_a != NULL && multi_sphere_b != NULL)
	{
		auto spheres_a = multi_sphere_a->get_spheres();
		auto spheres_b = multi_sphere_b->get_spheres();
		bool intersect = false;
		for (auto sa : spheres_a)
		{
			for (auto sb : spheres_b)
			{
				intersect |= is_intersection((BoundingSphere*)sa, (BoundingSphere*)sb);
			}
		}
		multi_sphere_a->free_spheres(spheres_a);
		multi_sphere_b->free_spheres(spheres_b);
		return intersect;
	}


	return false;
}

bool IntersectDetector::is_intersection(BoundingSphere* region_a, BoundingSphere* region_b)
{
	float intersect_distance = region_a->get_radius() + region_b->get_radius();
	vec3 offset_between = region_a->get_center() - region_b->get_center();

	return (glm::length(offset_between) <= intersect_distance);
}