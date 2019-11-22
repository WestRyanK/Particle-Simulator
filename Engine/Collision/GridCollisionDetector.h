#ifndef _GRID_COLLISION_DETECTOR_H_
#define _GRID_COLLISION_DETECTOR_H_

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

using namespace CodeMonkeys::Engine::Objects;
using namespace std;
using namespace glm;

namespace CodeMonkeys::Engine::Collision
{
    class GridCollisionDetector : public ICollisionDetector
    {
    private:
        float grid_size;
        set<Object3D*>** grid;
        map<Object3D*, ivec3> object_location_lookup;
        unsigned int x_count;
        unsigned int y_count;
        unsigned int z_count;
        void create_grid();
        ivec3 get_grid_cell_location(vec3 position);
        set<Object3D*>* get_grid_cell(ivec3 location);
    public:
        GridCollisionDetector(vec3 positive_bounds, vec3 negative_bounds, float grid_size);
        void clear();
        void insert(Object3D* object);
        void remove(Object3D* object);
        void update(Object3D* object);
        set<pair<Object3D*, Object3D*>> get_collisions();
    };
}

#endif