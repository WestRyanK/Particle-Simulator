#ifndef _ILIGHT_3D_H_
#define _ILIGHT_3D_H_

#include "../Dependencies/OpenGL.h"
#include <string>

namespace CodeMonkeys::Engine
{
	namespace Assets
	{
		class ShaderProgram;
	}
}

using namespace CodeMonkeys::Engine::Assets;
using namespace std;
using namespace glm;

namespace CodeMonkeys::Engine::Objects
{
    class ILight3D
    {
    protected:
        vec3 color;
        float strength;
        static string get_uniform_name(string light_type, unsigned int light_index, string property_name);
        // static void add_light_property_3v(ShaderProgram shader, string light_type, unsigned int light_index, string property_name, vec3 property_value);
        // static void add_light_property_1f(ShaderProgram shader, string light_type, unsigned int light_index, string property_name, float property_value);
    public:
        static void set_light_count(ShaderProgram* shader, string light_type, int light_count);
        ILight3D(vec3 color, float strength);
        // In order for a shader to use a light, it must add its data to the shader. For each frame, every shader must receive updated values for each light.
        virtual void add_light_to_shader(ShaderProgram* shader, unsigned int light_index) = 0;
        vec3 get_color();
        void set_color(vec3 color);
        float get_strength();
        void set_strength(float strength);
    };
}

#endif
