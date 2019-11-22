#include "AmbientLight.h"

#include "../Assets/ShaderProgram.h"

using CodeMonkeys::Engine::Objects::AmbientLight;
using namespace glm;


AmbientLight::AmbientLight(vec3 color, float strength) : Engine::Objects::ILight3D(color, strength)
{
}

void AmbientLight::add_light_to_shader(ShaderProgram* shader, unsigned int light_index)
{
    shader->setUniform(ILight3D::get_uniform_name("ambient", light_index, "color").c_str(), this->color);
    shader->setUniform(ILight3D::get_uniform_name("ambient", light_index, "strength").c_str(), this->strength);
}