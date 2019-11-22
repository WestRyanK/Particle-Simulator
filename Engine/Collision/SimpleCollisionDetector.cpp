#include "../Collision/SimpleCollisionDetector.h"

#include "../Objects/Object3D.h"

using CodeMonkeys::Engine::Collision::SimpleCollisionDetector;


SimpleCollisionDetector::SimpleCollisionDetector() : ICollisionDetector(vec3(), vec3())
{
    this->create_objects();
}

void SimpleCollisionDetector::clear()
{
    free(this->objects);
    this->create_objects();
}

void SimpleCollisionDetector::create_objects()
{
    this->objects = new set<Object3D*>();
    // this->objects = new vector<Object3D*>();
}

void SimpleCollisionDetector::insert(Object3D* object)
{
    this->objects->insert(object);
    // this->objects->push_back(object);
}

void SimpleCollisionDetector::remove(Object3D* object)
{
    this->objects->erase(object);
    // auto iter = std::find(this->objects->begin(), this->objects->end(), object);
    // if (iter != this->objects->end())
    //     this->objects->erase(iter);
}

void SimpleCollisionDetector::update(Object3D* object)
{
    this->remove(object);
    this->insert(object);
}

set<pair<Object3D*, Object3D*>> SimpleCollisionDetector::get_collisions()
{
    set<pair<Object3D*, Object3D*>> collisions;
    ICollisionDetector::get_collisions_among_objects(&collisions, this->objects, this->objects);
    return collisions;
}