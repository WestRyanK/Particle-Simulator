#include "../Objects/DirectionalLight.h"

#include "../Assets/ShaderProgram.h"
#include "../Dependencies/OpenGL.h"
#include "../Objects/ILight3D.h"

using CodeMonkeys::Engine::Objects::DirectionalLight;
using namespace CodeMonkeys::Engine::Objects;
using namespace glm;


DirectionalLight::DirectionalLight(vec3 color, float strength, vec3 direction) : ILight3D(color, strength)
{
	this->set_direction(direction);
}

vec3 DirectionalLight::get_direction()
{
    return this->direction;
}

void DirectionalLight::set_direction(vec3 direction)
{
    this->direction = glm::normalize(direction);
}

void DirectionalLight::add_light_to_shader(ShaderProgram* shader, unsigned int light_index)
{
    shader->setUniform(ILight3D::get_uniform_name("directional", light_index, "color").c_str(), this->color);
    shader->setUniform(ILight3D::get_uniform_name("directional", light_index, "strength").c_str(), this->strength);
    shader->setUniform(ILight3D::get_uniform_name("directional", light_index, "direction").c_str(), this->direction);
}