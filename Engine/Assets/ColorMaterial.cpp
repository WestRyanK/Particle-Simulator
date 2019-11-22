#include "../Assets/ColorMaterial.h"

#include "../Assets/ShaderProgram.h"

using CodeMonkeys::Engine::Assets::ColorMaterial;
using namespace CodeMonkeys::Engine::Assets;


ColorMaterial::ColorMaterial(ShaderProgram* shader, bool use_phong_highlight, float phong_exponent, vec3 phong_color, vec3 color) : Material(shader, use_phong_highlight, phong_exponent, phong_color)
{
    this->color = color;
}

void ColorMaterial::apply_material_to_shader()
{
    Material::apply_material_to_shader();
    this->apply_color_to_shader();
}

void ColorMaterial::apply_color_to_shader()
{
    this->shader->setUniform("object_color", this->color);
}

vec3 ColorMaterial::get_color()
{
    return this->color;
}

void ColorMaterial::set_color(vec3 color)
{
    this->color = color;
}

Material* ColorMaterial::clone()
{
    Material* material_clone = new ColorMaterial(this->shader, this->use_phong_highlight, this->phong_exponent, this->phong_color, this->color);
    return material_clone;
}