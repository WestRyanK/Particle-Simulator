#ifndef _OBJECT_3D_H_
#define _OBJECT_3D_H_

#include "../Dependencies/OpenGL.h"
#include <set>
#include <string>

namespace CodeMonkeys::Engine
{
	namespace Assets
	{
		class Model3D;
		class ShaderProgram;
	}
	namespace Collision
	{
		class ICollisionRegion;
	}
}

using namespace CodeMonkeys::Engine::Assets;
using namespace CodeMonkeys::Engine::Collision;
using namespace std;
using namespace glm;

namespace CodeMonkeys::Engine::Objects
{
	class Object3D
	{
	protected:
		Model3D* model;
		vec3 position;
		vec3 rotation;
		vec3 scale;
		set<Object3D*> children;
		Object3D* parent = NULL;
		std::string name;
	public:
		Object3D(Model3D* model, std::string name);
		~Object3D();

		vec3 get_position();
		void set_position(vec3 position);
		vec3 get_rotation();
		void set_rotation(vec3 rotation);
		void set_rotation(int axis, float value);
		vec3 get_scale();
		void set_scale(vec3 scale);
		void set_scale(float scale);
		vec3 get_transformed_position();
		mat4 get_hierarchical_transform();
		virtual mat4 get_transform();
		set<Object3D*> get_children();
		void add_child(Object3D* child);
		void remove_child(Object3D* child);
		Object3D* get_parent();
		void set_parent(Object3D* parent);
		std::string get_name();

		virtual ICollisionRegion* get_collision_region();

		virtual void update(float dt);
		virtual void draw(mat4 total_transform, ShaderProgram* shader);
	};
}

#endif