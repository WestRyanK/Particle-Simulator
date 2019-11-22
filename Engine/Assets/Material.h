#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include "../Dependencies/OpenGL.h"

namespace CodeMonkeys::Engine
{
	namespace Assets
	{
		class ShaderProgram;
	}
}

using namespace CodeMonkeys::Engine::Assets;
using namespace glm;

namespace CodeMonkeys::Engine::Assets
{
    class Material
    {
    protected:
        vec3 phong_color;
        float phong_exponent;
        bool use_phong_highlight = true;
        ShaderProgram* shader;
        //void set_material_type_id(unsigned int material_type_id);
    public:
        Material(ShaderProgram* shader, bool use_phong_highlight, float phong_exponent, vec3 phong_color);
        vec3 get_phong_color();
        float get_phong_exponent();
        bool get_use_phong_highlight();
        virtual Material* clone() = 0;
        ShaderProgram* get_shader();
        virtual void apply_material_to_shader();
        // This must be an id unique to the Material type;
        // Lets make ColorMaterial = 1 and TextureMaterial = 2;
        virtual int get_material_type_id() = 0;
        virtual void update(float dt);
    };
}

#endif