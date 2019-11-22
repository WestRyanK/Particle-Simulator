#ifndef _DIRECTIONAL_LIGHT_H_
#define _DIRECTIONAL_LIGHT_H_

#include "../Dependencies/OpenGL.h"
#include "../Objects/ILight3D.h"

namespace CodeMonkeys::Engine
{
	namespace Assets
	{
		class ShaderProgram;
	}
}

using namespace CodeMonkeys::Engine::Assets;
using namespace glm;

namespace CodeMonkeys::Engine::Objects
{
    class DirectionalLight : public ILight3D
    {
    private:
        vec3 direction;
    public:
        DirectionalLight(vec3 color, float strength, vec3 direction);
        vec3 get_direction();
        void set_direction(vec3 direction);
        virtual void add_light_to_shader(ShaderProgram* shader, unsigned int light_index);
    };
}

#endif