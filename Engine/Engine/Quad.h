#ifndef _QUAD_H_
#define _QUAD_H_

#include "../Dependencies/OpenGL.h"

namespace CodeMonkeys::Engine
{
	namespace Assets
	{
		class ShaderProgram;
	}
}

using namespace CodeMonkeys::Engine::Assets;

namespace CodeMonkeys::Engine::Engine
{
	class Quad
	{
	private:
		static ShaderProgram* quad_shader;
		static GLuint quad_vao;
		static void init();

	protected:
		GLuint quad_texture_id;
		vec2 position;
		vec2 size;

	public:
		Quad(vec2 position, vec2 size);
		virtual void draw();
		virtual void update(float dt);
		void set_texture(GLuint texture);
		GLuint get_texture();
	};
}

#endif