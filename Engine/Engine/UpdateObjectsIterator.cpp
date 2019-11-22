#include "UpdateObjectsIterator.h"

#include "../Collision/ICollisionDetector.h"
#include "../Objects/Object3D.h"

using CodeMonkeys::Engine::Engine::UpdateObjectsIterator;


void UpdateObjectsIterator::visit_object(Object3D* object)
{
    if (object != NULL)
    {
        object->update(this->dt);
        this->collision_detector->update(object);
    }
}

void UpdateObjectsIterator::post_visit_children(Object3D* parent)
{
}

void UpdateObjectsIterator::update(Object3D* world_root, float dt)
{
    this->dt = dt;
    this->traverse(world_root);
}

void UpdateObjectsIterator::set_collision_detector(ICollisionDetector* collision_detector)
{
    this->collision_detector = collision_detector;
}
