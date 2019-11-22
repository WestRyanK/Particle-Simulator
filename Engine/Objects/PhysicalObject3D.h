#ifndef _PHYSICAL_OBJECT_3D_H_
#define _PHYSICAL_OBJECT_3D_H_

#include "../Dependencies/OpenGL.h"
#include "../Objects/Object3D.h"
#include <string>

namespace CodeMonkeys::Engine
{
	namespace Assets
	{
		class Model3D;
	}
}

using namespace CodeMonkeys::Engine::Assets;
using namespace glm;

namespace CodeMonkeys::Engine::Objects
{
    class PhysicalObject3D: public Object3D
    {
    protected:
        vec3 velocity;
        vec3 angular_velocity;
        vec3 acceleration;

    public:
        PhysicalObject3D(Model3D* model, std::string name);

        vec3 get_velocity();
        vec3 get_acceleration();
        void set_velocity(vec3 velocity);
        vec3 get_angular_velocity();
        void set_angular_velocity(vec3 angular_velocity);
        virtual void update(float dt);

        void set_velocity(int axis, float value);
    };
}

#endif
