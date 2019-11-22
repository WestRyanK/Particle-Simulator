#ifndef _RENDERER_H_
#define _RENDERER_H_

#include "../Dependencies/OpenGL.h"
#include <set>

namespace CodeMonkeys::Engine
{
	namespace Assets
	{
		class ShaderProgram;
	}
	namespace Engine
	{
		class Quad;
		class DrawObjectsIterator;
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
using namespace glm;
using namespace std;

namespace CodeMonkeys::Engine::Engine
{
    class Renderer
    {
    private:
        DrawObjectsIterator* draw_objects_iterator;
        GLuint width;
        GLuint height;
        GLFWwindow* window;

    protected:
        GLuint get_width();
        GLuint get_height();
        GLFWwindow* get_window();


        void clear();
        void draw_quads(set<Quad*> quads);
        void set_lighting(ShaderProgram* shader, set<ILight3D*> lights);
        void set_camera(ShaderProgram* shader, Camera3D* camera);
        void draw_objects(Object3D* world_root, set<Quad*> quads, ShaderProgram* shader);

    public:
        Renderer(GLFWwindow* window, GLuint width, GLuint height);
        virtual void render(set<ShaderProgram*> shaders, set<ILight3D*> lights, Camera3D* camera, Object3D* world_root, Skybox* skybox, set<Quad*> quads);
    };
}

#endif