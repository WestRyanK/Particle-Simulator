#include "../Assets/Material.h"

#include "../Assets/ShaderProgram.h"

using CodeMonkeys::Engine::Assets::Material;
using namespace CodeMonkeys::Engine::Assets;


Material::Material(ShaderProgram* shader, bool use_phong_highlight, float phong_exponent, vec3 phong_color)
{
    this->shader = shader;
    this->phong_color = phong_color;
    this->phong_exponent = phong_exponent;
    this->use_phong_highlight = use_phong_highlight;
}

vec3 Material::get_phong_color()
{
    return this->phong_color;
}

float Material::get_phong_exponent()
{
    return this->phong_exponent;
}

bool Material::get_use_phong_highlight()
{
    return this->use_phong_highlight;
}

ShaderProgram* Material::get_shader()
{
    return this->shader;
}

void Material::apply_material_to_shader()
{
    this->shader->setUniform("material_type_id", (int)this->get_material_type_id());
    this->shader->setUniform("phong_color", this->phong_color);
    this->shader->setUniform("phong_exponent", this->phong_exponent);
    this->shader->setUniform("use_phong_highlight", this->use_phong_highlight);
}

void Material::update(float dt)
{
    
}