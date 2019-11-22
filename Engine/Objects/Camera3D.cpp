#include "../Objects/Camera3D.h"

#include "../Assets/ShaderProgram.h"
#include "../Objects/Object3D.h"

using CodeMonkeys::Engine::Objects::Camera3D;
using namespace CodeMonkeys::Engine::Objects;
using namespace glm;


Camera3D::Camera3D() : Object3D(NULL, "Camera")
{
    this->look_at = vec3(0.0f, 0.0f, 0.0f);
    this->position = vec3(0.0f, 0.0f, 60.0f);
    // TODO: Don't hardcode window size.
    this->perspective_projection = glm::perspective(glm::radians(45.0f), 640.0f / 480.0f, 0.01f, 4600.0f);
}

vec3 Camera3D::get_look_at()
{
    return this->look_at;
}

void Camera3D::set_look_at(Object3D* look_at_parent)
{
    this->look_at_parent = look_at_parent;
}

void Camera3D::set_look_at(vec3 look_at)
{
    this->look_at = look_at;
}

vec3 Camera3D::get_up()
{
    return this->up;
}

void Camera3D::set_up(vec3 up)
{
    this->up = up;
}

void Camera3D::update_shader_with_camera(ShaderProgram* shader)
{
    mat4 view = this->get_view_transform();
    vec4 hierarchical_position = this->get_hierarchical_transform() * vec4(0,0,0,1);

    shader->setUniform("view_transform", view);
    shader->setUniform("projection_transform", this->perspective_projection);
    shader->setUniform("camera_position", vec3(hierarchical_position.x, hierarchical_position.y, hierarchical_position.z));
}

Object3D* Camera3D::get_look_at_parent()
{
    return this->look_at_parent;
}

mat4 Camera3D::get_perpective_projection()
{
    return this->perspective_projection;
}


mat4 Camera3D::get_view_transform()
{
    mat4 hierarchical_transform = this->get_hierarchical_transform();
    vec3 transformed_look_at;
    if (this->look_at_parent != NULL)
    {
        mat4 look_hierarchical_transform = this->look_at_parent->get_hierarchical_transform();
        transformed_look_at = vec3(look_hierarchical_transform * vec4(transformed_look_at, 1.0f));
    }
    else
    {
        transformed_look_at = this->look_at;
        // transformed_look_at = vec3(hierarchical_transform * vec4(this->look_at, 1));
    }
    vec3 transformed_position = vec3(hierarchical_transform * vec4(0,0,0, 1));

    return glm::lookAt(transformed_position, transformed_look_at, this->up);
}

// SFL 223 Notes:
//braising - moist heat method of cooking less tender cuts of meat
void Camera3D::control(std::string control_name, float value, float dt)
{
    const float velocity = 2.0f;
    const float angular_velocity = 10.0f;
    vec3 forward = glm::normalize(this->look_at - this->position);
    vec3 sideways = glm::normalize(glm::cross(this->up, forward));
    if (control_name == "move_x")
    {
        this->position += sideways * dt * value * velocity;
        //this->look_at += sideways * dt * value * velocity;
    }
    if (control_name == "move_y")
    {
		this->position += this->up * dt * value * velocity;
		this->look_at += this->up * dt * value * velocity;
        //this->position.y -= value * dt * velocity;
        //this->look_at.y -= value * dt * velocity;
    }
    if (control_name == "move_z")
    {
        this->position += forward * dt * value * velocity;
        //this->look_at += forward * dt * value * velocity;
    }
    if (control_name == "rotate_y")
    {
        mat4 transform = mat4(1.0);
        transform = glm::rotate(transform, value * glm::radians(angular_velocity), this->up);

        vec4 rotate = vec4(forward.x, forward.y, forward.z, 1.0f) * transform;
        vec3 rotate3 = vec3(rotate.x, rotate.y, rotate.z);
        this->look_at = rotate3 + this->position;
    }
}

void Camera3D::control(std::string control_name, int value, float dt)
{

}
