#ifndef _ICOLLISION_DETECTOR_H_
#define _ICOLLISION_DETECTOR_H_

#include "../Dependencies/OpenGL.h"
#include <set>
#include <utility>

namespace CodeMonkeys::Engine
{
	namespace Collision
	{
		class ICollisionRegion;
	}
	namespace Objects
	{
		class Object3D;
	}
}

using namespace CodeMonkeys::Engine::Collision;
using namespace CodeMonkeys::Engine::Objects;
using namespace std;
using namespace glm;

namespace CodeMonkeys::Engine::Collision
{
	class ICollisionDetector
	{
	private:
		vec3 positive_bounds;
		vec3 negative_bounds;
		bool is_duplicate(set<pair<Object3D*, Object3D*>>* collisions, Object3D* object_a, Object3D* object_b);
	protected:
		//vec3 get_transformed_position(Object3D* object);
		void get_collisions_among_objects(set<pair<Object3D*, Object3D*>>* collisions, set<Object3D*>* objects_a, set<Object3D*>* objects_b);
	public:
		ICollisionDetector(vec3 positive_bounds, vec3 negative_bounds);
		virtual void clear() = 0;
		virtual void insert(Object3D* object) = 0;
		virtual void remove(Object3D* object) = 0;
		virtual void update(Object3D* object) = 0;
		virtual set<pair<Object3D*, Object3D*>> get_collisions() = 0;

		vec3 get_negative_bounds();
		vec3 get_positive_bounds();
	};
}

#endif
