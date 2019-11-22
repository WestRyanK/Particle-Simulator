#include "../Engine/Renderer3D.h"

#include "../Assets/ShaderProgram.h"
#include "../Engine/Quad.h"
#include "../Objects/Camera3D.h"
#include "../Objects/ILight3D.h"
#include "../Objects/Object3D.h"
#include "../Objects/Skybox.h"

using CodeMonkeys::Engine::Engine::Renderer3D;
using namespace CodeMonkeys::Engine::Assets;


Renderer3D::Renderer3D(GLFWwindow* window, int width, int height, float camera_spacing) : FrameBufferRenderer(window, width, height)
{
    this->camera_spacing = camera_spacing;
    this->create_frame_buffer(this->get_width() / 2, this->get_height(), this->frame_buffers[0], this->rendered_textures[0]);
    this->create_frame_buffer(this->get_width() / 2, this->get_height(), this->frame_buffers[1], this->rendered_textures[1]);
    this->quads[0] = new Quad(vec2(-1.0f, -1.0f), vec2(1.0f, 2.0f));
    this->quads[1] = new Quad(vec2(0.0f, -1.0f), vec2(1.0f, 2.0f));
}

vector<Camera3D*> Renderer3D::create_3d_cameras(Camera3D* camera)
{
    vector<Camera3D*> cameras;

    cameras.push_back(new Camera3D());
    cameras.push_back(new Camera3D());
    vec3 sideways = normalize(cross(camera->get_up(), camera->get_look_at() - camera->get_position()));
    vec3 offset = sideways * this->camera_spacing;

    for (unsigned int i = 0; i < 2; i++)
    {
        cameras[i]->set_look_at(camera->get_look_at_parent());
        cameras[i]->set_parent(camera->get_parent());
        cameras[i]->set_position(camera->get_position());
        cameras[i]->set_position(camera->get_position() - 2.0f * offset * (float)i + offset );
    }

    return cameras;
}

void Renderer3D::render(set<ShaderProgram*> shaders, set<ILight3D*> lights, Camera3D* camera, Object3D* world_root, Skybox* skybox, set<Quad*> quads)
{
    vector<Camera3D*> cameras = create_3d_cameras(camera);

    GLuint half_width = this->get_width() / 2;

    for(unsigned int i = 0; i < cameras.size(); i++)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, this->frame_buffers[i]);
        glViewport(0, 0, this->get_width() / 2, this->get_height()); 
        this->clear();

        if (skybox != NULL)
            skybox->draw(cameras[i]->get_view_transform(), cameras[i]->get_perpective_projection());
        for (ShaderProgram* shader : shaders)
        {
            shader->use_program();

            this->set_lighting(shader, lights); // TODO: investigate 1282 error
            this->set_camera(shader, cameras[i]); // TODO: investigate 1282 error

            this->draw_objects(world_root, quads, shader);
        }
    }

    this->draw_frame_buffer(this->rendered_texture);
    this->quads[0]->set_texture(rendered_textures[0]);
    this->quads[0]->draw();
    this->quads[1]->set_texture(rendered_textures[1]);
    this->quads[1]->draw();
    glfwSwapBuffers(this->get_window());

    free(cameras[0]);
    free(cameras[1]);
}