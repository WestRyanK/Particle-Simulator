#pragma once

#include "../Dependencies/OpenGL.h"
#include "../Objects/PhysicalObject3D.h"
#include <string>

namespace CodeMonkeys::Engine
{
	namespace Assets
	{
		class Texture;
	}
}

using namespace CodeMonkeys::Engine::Assets;
using namespace std;

namespace CodeMonkeys::Engine::Objects
{
    class Billboard : public PhysicalObject3D
    {
    private:
        static ShaderProgram* billboard_shader;
        static GLuint* billboard_vao;

    protected:
        Texture* billboard_texture;
        float width;
        float height;
    public:
        Billboard(std::string name, Texture* billboard_texture, float width, float height);

        virtual void draw(mat4 total_transform, ShaderProgram* shader);
        static ShaderProgram* get_shader();
        void update(float dt);
        static void init_billboard_class();
        void reset();
        float get_width();
        void set_width(float width);
        float get_height();
        void set_height(float height);
        Billboard* clone();
    };
}