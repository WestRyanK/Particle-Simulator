#ifndef _DRAW_OBJECTS_ITERATOR_H_
#define _DRAW_OBJECTS_ITERATOR_H_

#include "../Dependencies/OpenGL.h"
#include "WorldObjectsIterator.h"
#include <stack>

namespace CodeMonkeys::Engine
{
	namespace Assets
	{
		class ShaderProgram;
	}
	namespace Objects
	{
		class Object3D;
	}
}

using namespace CodeMonkeys::Engine::Assets;
using namespace CodeMonkeys::Engine::Objects;
using namespace glm;

namespace CodeMonkeys::Engine::Engine
{
	class DrawObjectsIterator : public WorldObjectsIterator
	{
	private:
		ShaderProgram* current_shader = nullptr;
		std::stack<mat4> transform_stack;
		virtual void visit_object(Objects::Object3D* object);
		virtual void post_visit_children(Objects::Object3D* parent);
	public:
		void draw(Objects::Object3D* world_root, ShaderProgram* current_shader);
	};
}

#endif