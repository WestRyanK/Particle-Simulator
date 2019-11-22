#include "../Objects/Billboard.h"

#include "../Assets/AnimatedTexture.h"
#include "../Assets/ShaderProgram.h"
#include "../Assets/Texture.h"
#include "../Engine/Quad.h"

using CodeMonkeys::Engine::Objects::Billboard;
using namespace CodeMonkeys::Engine::Assets;
using namespace CodeMonkeys::Engine::Engine;


ShaderProgram* Billboard::billboard_shader = NULL;
GLuint* Billboard::billboard_vao = NULL;

Billboard::Billboard(std::string name, Texture* billboard_texture, float width, float height) : PhysicalObject3D(NULL, name)
{
    if (Billboard::billboard_shader == NULL && billboard_vao == NULL)
    {
        Billboard::init_billboard_class();
    }

    this->billboard_texture = billboard_texture;
    this->width = width;
    this->height = height;
}

void Billboard::init_billboard_class()
{
    Billboard::billboard_shader = new ShaderProgram("Assets/Shaders/billboard.vert", "Assets/Shaders/billboard.frag");

    Billboard::billboard_vao = new GLuint();
    glGenVertexArrays(1, Billboard::billboard_vao);
    glBindVertexArray(*Billboard::billboard_vao);

    float v = 0.5f;
    const GLfloat billboard_vertex_buffer_data[] = {
        -v, -v, 0.0f, 0.0f,
        v, -v,  1.0f, 0.0f,
        -v, v,  0.0f, 1.0f,
        -v, v,  0.0f, 1.0f,
        v, -v,  1.0f, 0.0f,
        v, v,   1.0f, 1.0f
    };

    GLuint billboard_vbo;
    glGenBuffers(1, &billboard_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, billboard_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(billboard_vertex_buffer_data), billboard_vertex_buffer_data, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    glBindVertexArray(0);
}

void Billboard::draw(mat4 transform, ShaderProgram* shader)
{
    if (Billboard::billboard_shader == shader)
    {
        glDepthMask(GL_FALSE);
        Billboard::billboard_shader->setUniform("object_transform", transform);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, this->billboard_texture->get_texture_id());
        GLuint billboard_texture_location = glGetUniformLocation(Billboard::billboard_shader->get_shader_id(), "billboard_texture");
        glUniform1i(billboard_texture_location, 0);

        billboard_shader->setUniform("billboard_size", vec2(this->width, this->height));

        glBindVertexArray(*Billboard::billboard_vao);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
        glDepthMask(GL_TRUE);
    }
}

ShaderProgram* Billboard::get_shader()
{
    return Billboard::billboard_shader;
}

void Billboard::update(float dt)
{
    PhysicalObject3D::update(dt);
    this->billboard_texture->update(dt);
}

void Billboard::reset()
{
    this->billboard_texture->reset();
}

float Billboard::get_width()
{
    return this->width;
}

void Billboard::set_width(float width)
{
    this->width = width;
}

float Billboard::get_height()
{
    return this->height;
}

void Billboard::set_height(float height)
{
    this->height = height;
}

Billboard* Billboard::clone()
{
    Texture* texture_clone = NULL;
    if (this->billboard_texture != NULL)
        texture_clone = this->billboard_texture->clone();
    Billboard* billboard_clone = new Billboard(this->name, texture_clone, this->width, this->height);
    return billboard_clone;
}