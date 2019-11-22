#ifndef _RENDERER_3D_H_
#define _RENDERER_3D_H_

#include "../Dependencies/OpenGL.h"
#include "../Engine/FrameBufferRenderer.h"
#include <vector>

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
		class Camera3D;
		class ILight3D;
		class Object3D;
		class Skybox;
	}
}

using namespace CodeMonkeys::Engine::Assets;
using namespace CodeMonkeys::Engine::Engine;
using namespace CodeMonkeys::Engine::Objects;
using namespace glm;

namespace CodeMonkeys::Engine::Engine
{
    class Renderer3D : public FrameBufferRenderer
    {
    private:
        float camera_spacing;
        Quad* quads[2];
        GLuint frame_buffers[2];
        GLuint rendered_textures[2];

        vector<Camera3D*> create_3d_cameras(Camera3D* camera);
    public:
        Renderer3D(GLFWwindow* window, int width, int height, float camera_spacing);
        void render(set<ShaderProgram*> shaders, set<ILight3D*> lights, Camera3D* camera, Object3D* world_root, Skybox* skybox, set<Quad*> quads);
    };
}

#endif