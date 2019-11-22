#ifndef _SIMPLE_COLLISION_DETECTOR_H_
#define _SIMPLE_COLLISION_DETECTOR_H_

#include "../Collision/ICollisionDetector.h"
#include <map>
#include <set>

namespace CodeMonkeys::Engine
{
	namespace Objects
	{
		class Object3D;
	}
}

using namespace std;
using namespace CodeMonkeys::Engine::Objects;

namespace CodeMonkeys::Engine::Collision
{
    class SimpleCollisionDetector : public ICollisionDetector
    {
    private:
        set<Object3D*>* objects;
        // vector<Object3D*>* objects;
        void create_objects();
    public:
        SimpleCollisionDetector();
        void clear();
        void insert(Object3D* object);
        void remove(Object3D* object);
        void update(Object3D* object);
        set<pair<Object3D*, Object3D*>> get_collisions();
    };
}

#endif