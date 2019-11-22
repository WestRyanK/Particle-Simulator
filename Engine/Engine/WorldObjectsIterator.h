#ifndef _WORLD_OBJECTS_ITERATOR_H_
#define _WORLD_OBJECTS_ITERATOR_H_

#include "../Dependencies/OpenGL.h"

namespace CodeMonkeys::Engine
{
	namespace Objects
	{
		class Object3D;
	}
}
using namespace CodeMonkeys::Engine::Objects;

namespace CodeMonkeys::Engine::Engine
{
	class WorldObjectsIterator
	{
	private:

	protected:
		void traverse(Object3D* object);
		virtual void visit_object(Object3D* object) = 0;
		virtual void post_visit_children(Object3D* parent) = 0;
	};
}

#endif