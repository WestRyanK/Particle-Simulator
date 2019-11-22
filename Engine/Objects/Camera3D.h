#ifndef _CAMERA_3D_H_
#define _CAMERA_3D_H_

#include "../Control/IControllable.h"
#include "../Dependencies/OpenGL.h"
#include "../Objects/Object3D.h"

using namespace CodeMonkeys::Engine::Control;
using namespace glm;

namespace CodeMonkeys::Engine::Objects
{
    class Camera3D : public Object3D, public IControllable
    {
    private:
        vec3 look_at;
        vec3 up = vec4(0.0f, 1.0f, 0.0f, 1.0f);
        mat4 perspective_projection;
        Object3D* look_at_parent = NULL;

    public:
        Camera3D();
        vec3 get_look_at();
        void set_look_at(vec3 look_at);
        vec3 get_up();
        void set_up(vec3 up);
        void set_look_at(Object3D* look_at_parent);
        Object3D* get_look_at_parent();
        
        mat4 get_view_transform();
        mat4 get_perpective_projection();

        void update_shader_with_camera(ShaderProgram* shader);

        void control(std::string control_name, float value, float dt);
        void control(std::string control_name, int value, float dt);
    };
}

#endif
