#pragma once

#include "ILight3D.h"

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
    class AmbientLight : public ILight3D
    {
    public:
        AmbientLight(vec3 color, float strength);
        virtual void add_light_to_shader(ShaderProgram* shader, unsigned int light_index);
    };
}