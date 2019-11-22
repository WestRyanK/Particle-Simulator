#include "../Engine/Quad.h"

#include "../Assets/ShaderProgram.h"
#include "../Assets/Texture.h"

using CodeMonkeys::Engine::Engine::Quad;


ShaderProgram* Quad::quad_shader = NULL;
GLuint Quad::quad_vao = -1;

// Quad::Quad(float x, float y, float width, float height)
Quad::Quad(vec2 position, vec2 size)
{
	this->position = position;
	this->size = size;

	if (Quad::quad_shader == NULL)
		Quad::init();
}

void Quad::init()
{
	Quad::quad_shader = new ShaderProgram("Assets/Shaders/quad.vert", "Assets/Shaders/quad.frag");

	// The fullscreen quad's FBO
	glGenVertexArrays(1, &Quad::quad_vao);
	glBindVertexArray(Quad::quad_vao);

	const float size = 1;
	const GLfloat quad_vertex_buffer_data[] = {
		0, 0,         0.0f, 0.0f,
		size, 0,      1.0f, 0.0f,
		0,  size,     0.0f, 1.0f,
		0,  size,     0.0f, 1.0f,
		size, 0,      1.0f, 0.0f,
		size,  size,  1.0f, 1.0f
	};

	// const float size = 0.5f;
	// const GLfloat quad_vertex_buffer_data[] = {
	//     -size, -size, 0.0f, 0.0f,
	//     size, -size,  1.0f, 0.0f,
	//     -size,  size, 0.0f, 1.0f,
	//     -size,  size, 0.0f, 1.0f,
	//     size, -size,  1.0f, 0.0f,
	//     size,  size,  1.0f, 1.0f
	// };

	GLuint quad_vbo;
	glGenBuffers(1, &quad_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, quad_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertex_buffer_data), quad_vertex_buffer_data, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	glBindVertexArray(0);
}

void Quad::draw()
{
	glDepthMask(GL_FALSE);
	glDepthFunc(GL_ALWAYS);
	Quad::quad_shader->use_program();

	// // Bind our texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, this->quad_texture_id);
	Quad::quad_shader->setUniform("quad_texture", (int)0);
	Quad::quad_shader->setUniform("position", this->position);
	Quad::quad_shader->setUniform("size", this->size);

	glBindVertexArray(this->quad_vao);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glBindVertexArray(0);
	// glBindTexture(GL_TEXTURE_2D, 0);
	glDepthFunc(GL_LESS);
	glDepthMask(GL_TRUE);
}

void Quad::set_texture(GLuint texture)
{
	this->quad_texture_id = texture;
}

GLuint Quad::get_texture()
{
	return this->quad_texture_id;
}

void Quad::update(float dt)
{

}