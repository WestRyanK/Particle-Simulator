#ifndef _TEXTURE_MATERIAL_H_
#define _TEXTURE_MATERIAL_H_

#include "../Assets/ColorMaterial.h"
#include "../Dependencies/OpenGL.h"

using namespace glm;

namespace CodeMonkeys::Engine
{
	namespace Assets
	{
		class Texture;
	}
}

using namespace CodeMonkeys::Engine::Assets;

namespace CodeMonkeys::Engine::Assets
{
    class TextureMaterial : public ColorMaterial
    {
    protected:
        Texture* texture = NULL;
    public:
        TextureMaterial(ShaderProgram* shader, bool use_phong_highlight, float phong_exponent, vec3 phong_color, vec3 color, Texture* texture);
        TextureMaterial(ShaderProgram* shader, bool use_phong_highlight, float phong_exponent, vec3 phong_color, Texture* texture);
        Texture* get_texture();
        virtual Material* clone();
        virtual void apply_material_to_shader();
        virtual int get_material_type_id() { return 2; };
        virtual void update(float dt);
    };
}

#endif