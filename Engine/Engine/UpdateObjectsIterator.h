#ifndef _UPDATE_OBJECTS_ITERATOR_H_
#define _UPDATE_OBJECTS_ITERATOR_H_

#include "WorldObjectsIterator.h"

namespace CodeMonkeys::Engine
{
	namespace Collision
	{
		class ICollisionDetector;
	}
	namespace Objects
	{
		class Object3D;
	}
}

using namespace CodeMonkeys::Engine::Collision;
using namespace CodeMonkeys::Engine::Objects;

namespace CodeMonkeys::Engine::Engine
{
    class UpdateObjectsIterator : public WorldObjectsIterator
    {
    private:
        float dt = 0.0f;
        virtual void visit_object(Object3D* object);
        virtual void post_visit_children(Object3D* parent);
        ICollisionDetector* collision_detector = nullptr;
    public:
        void set_collision_detector(ICollisionDetector* collision_detector);
        void update(Object3D* world_root, float dt);
    };
}

#endif