#include "../Engine/FrameBufferRenderer.h"

#include "../Assets/ShaderProgram.h"
#include "../Engine/Quad.h"
#include "../Objects/Camera3D.h"
#include "../Objects/ILight3D.h"
#include "../Objects/Object3D.h"
#include "../Objects/Skybox.h"

using CodeMonkeys::Engine::Engine::FrameBufferRenderer;


FrameBufferRenderer::FrameBufferRenderer(GLFWwindow* window, GLuint width, GLuint height) : Renderer(window, width, height)
{
    this->create_frame_buffer(this->get_width(), this->get_height(), this->frame_buffer, this->rendered_texture);
    this->quad = new Quad(vec2(-1.0f, -1.0f), vec2(2.0f, 2.0f));
}

void FrameBufferRenderer::draw_frame_buffer(GLuint rendered_texture)
{ 
    glViewport(0, 0, this->get_width(), this->get_height()); 
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, this->get_width(), this->get_height()); 
    // // Clear the screen
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

}

void FrameBufferRenderer::create_frame_buffer(GLuint width, GLuint height, GLuint& frame_buffer, GLuint& rendered_texture)
{
    frame_buffer = 0;
    glGenFramebuffers(1, &frame_buffer);
    glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);

    // The texture we're going to render to
    glGenTextures(1, &rendered_texture);
    glBindTexture(GL_TEXTURE_2D, rendered_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);
    // Set "rendered_texture" as our colour attachement #0
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, rendered_texture, 0);

    // The depth buffer
    GLuint rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

    // Set the list of draw buffers.
    GLenum draw_buffers[1] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, draw_buffers);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        printf("Failed to load framebuffer!");
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

#include <fstream>
void screenshot (char filename[160],int x, int y)
{
    // get the image data
    long imageSize = x * y * 3;
    unsigned char *data = new unsigned char[imageSize];
    glReadPixels(0,0,x,y, GL_BGR,GL_UNSIGNED_BYTE,data);// split x and y sizes into bytes
    int xa= x % 256;
    int xb= (x-xa)/256;int ya= y % 256;
    int yb= (y-ya)/256;//assemble the header
    unsigned char header[18]={0,0,2,0,0,0,0,0,0,0,0,0,(unsigned char)xa,(unsigned char)xb,(unsigned char)ya,(unsigned char)yb,24,0};
    // write header and data to file
    fstream File(filename, ios::out | ios::binary);
    File.write (reinterpret_cast<char *>(header), sizeof (char)*18);
    File.write (reinterpret_cast<char *>(data), sizeof (char)*imageSize);
    File.close();

    delete[] data;
    data=NULL;
}

void FrameBufferRenderer::render(set<ShaderProgram*> shaders, set<ILight3D*> lights, Camera3D* camera, Object3D* world_root, Skybox* skybox, set<Quad*> quads)
{
    glBindFramebuffer(GL_FRAMEBUFFER, this->frame_buffer);
    glViewport(0, 0, this->get_width(), this->get_height()); 

    this->clear();

    if (skybox != NULL)
        skybox->draw(camera->get_view_transform(), camera->get_perpective_projection());
    for (ShaderProgram* shader : shaders)
    {
        shader->use_program();

        this->set_lighting(shader, lights); // TODO: investigate 1282 error
        this->set_camera(shader, camera); // TODO: investigate 1282 error

        this->draw_objects(world_root, quads, shader);
    }

    this->draw_frame_buffer(this->rendered_texture);
    this->quad->set_texture(this->rendered_texture);
    this->quad->draw();
    glfwSwapBuffers(this->get_window());
}
