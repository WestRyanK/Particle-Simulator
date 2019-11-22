#include "../Collision/ICollisionDetector.h"

#include "../Collision/ICollisionRegion.h"
#include "../Collision/IntersectDetector.h"
#include "../Objects/Object3D.h"

using CodeMonkeys::Engine::Collision::ICollisionDetector;
using namespace CodeMonkeys::Engine::Objects;


ICollisionDetector::ICollisionDetector(vec3 positive_bounds, vec3 negative_bounds)
{
    this->positive_bounds = positive_bounds;
    this->negative_bounds = negative_bounds;
}

vec3 ICollisionDetector::get_negative_bounds()
{
    return this->negative_bounds;
}

vec3 ICollisionDetector::get_positive_bounds()
{
    return this->positive_bounds;
}

void ICollisionDetector::get_collisions_among_objects(set<pair<Object3D*, Object3D*>>* collisions, set<Object3D*>* objects_a, set<Object3D*>* objects_b)
{
    for (auto object_a_iter = objects_a->begin(); object_a_iter != objects_a->end(); object_a_iter++)
    {
        for (auto object_b_iter = objects_b->begin(); object_b_iter != objects_b->end(); object_b_iter++)
        {
            if (*object_a_iter != *object_b_iter)
            {
                if(IntersectDetector::is_intersection((*object_a_iter)->get_collision_region(), (*object_b_iter)->get_collision_region()))
                {
                    if (!this->is_duplicate(collisions, (*object_a_iter), (*object_b_iter)))
                    {
                        collisions->insert(pair<Object3D*, Object3D*>(*object_a_iter, *object_b_iter));
                    }
                }
            }
        }
    }
}

bool ICollisionDetector::is_duplicate(set<pair<Object3D*, Object3D*>>* collisions, Object3D* object_a, Object3D* object_b)
{
    for (auto collision : *collisions)
    {
        if ((object_a == collision.first && object_b == collision.second) || (object_a == collision.second && object_b == collision.first))
            return true;
    }
    return false;
}