#include "ILight3D.h"

#include "../Assets/ShaderProgram.h"
#include "../Dependencies/OpenGL.h"
#include <iostream>
#include <sstream>
#include <string>

using CodeMonkeys::Engine::Objects::ILight3D;
using namespace glm;
using namespace std;


ILight3D::ILight3D(vec3 color, float strength)
{
    this->color = color;
    this->strength = strength;
}

vec3 ILight3D::get_color()
{
    return this->color;
}

void ILight3D::set_color(vec3 color)
{
    this->color = color;
}

float ILight3D::get_strength()
{
    return this->strength;
}

void ILight3D::set_strength(float strength)
{
    this->strength = strength;
}

string ILight3D::get_uniform_name(string light_type, unsigned int light_index, string property_name)
{
    std::ostringstream ss;
    ss << light_type << "[" << light_index << "]." << property_name;
    std::string uniform_name = ss.str();
    return uniform_name;
}

void ILight3D::set_light_count(ShaderProgram* shader, string light_type, int light_count)
{
    std::ostringstream ss;
    ss << light_type << "_count";
    std::string uniform_name = ss.str();
    shader->setUniform(uniform_name.c_str(), light_count);
}