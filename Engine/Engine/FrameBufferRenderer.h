#ifndef _FRAME_BUFFER_RENDERER_H_
#define _FRAME_BUFFER_RENDERER_H_

#include "../Engine/Renderer.h"

namespace CodeMonkeys::Engine
{
	namespace Assets
	{
		class ShaderProgram;
	}
	namespace Engine
	{
		class Quad;
	}
	namespace Objects
	{
		class ILight3D;
		class Camera3D;
		class Object3D;
		class Skybox;
	}
}

using namespace CodeMonkeys::Engine::Assets;
using namespace CodeMonkeys::Engine::Engine;
using namespace CodeMonkeys::Engine::Objects;

namespace CodeMonkeys::Engine::Engine
{
    class FrameBufferRenderer : public Renderer
    {
    private:
        Quad* quad;

    protected:
        GLuint frame_buffer;
        GLuint rendered_texture;

        void create_frame_buffer(GLuint width, GLuint height, GLuint& frame_buffer, GLuint& rendered_texture);
        void draw_frame_buffer(GLuint rendered_texture);
    
    public:
        FrameBufferRenderer(GLFWwindow* window, GLuint width, GLuint height);
        virtual void render(set<ShaderProgram*> shaders, set<ILight3D*> lights, Camera3D* camera, Object3D* world_root, Skybox* skybox, set<Quad*> quads);

    };
}

#endif