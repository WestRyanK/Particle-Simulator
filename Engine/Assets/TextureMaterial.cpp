#include "../Assets/TextureMaterial.h"

#include "../Assets/ShaderProgram.h"
#include "../Assets/Texture.h"

using CodeMonkeys::Engine::Assets::TextureMaterial;
using namespace CodeMonkeys::Engine::Assets;


TextureMaterial::TextureMaterial(ShaderProgram* shader, bool use_phong_highlight, float phong_exponent, vec3 phong_color, vec3 color, Texture* texture) 
    : ColorMaterial(shader, use_phong_highlight, phong_exponent, phong_color, color)
{
    this->texture = texture;
}

TextureMaterial::TextureMaterial(ShaderProgram* shader, bool use_phong_highlight, float phong_exponent, vec3 phong_color, Texture* texture) 
    : TextureMaterial(shader, use_phong_highlight, phong_exponent, phong_color, vec3(1), texture)
{
}

void TextureMaterial::apply_material_to_shader()
{
    Material::apply_material_to_shader();
    ColorMaterial::apply_color_to_shader();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, this->texture->get_texture_id());
    this->shader->setUniform("texture_0", 0);
}

Material* TextureMaterial::clone()
{
    Material* material_clone = new TextureMaterial(this->shader, this->use_phong_highlight, this->phong_exponent, this->phong_color, this->color, this->texture->clone());
    return material_clone;
}

void TextureMaterial::update(float dt)
{
    if (this->texture != NULL)
    {
        this->texture->update(dt);
    }
}

Texture* TextureMaterial::get_texture()
{
	return this->texture;
}
