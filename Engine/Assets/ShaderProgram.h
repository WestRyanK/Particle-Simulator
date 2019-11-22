#ifndef _SHADER_PROGRAM_H_
#define _SHADER_PROGRAM_H_

#include "../Dependencies/OpenGL.h"
#include <string>

using namespace glm;
using namespace std;

namespace CodeMonkeys::Engine::Assets
{
	class ShaderProgram
	{
	private:
		unsigned int shader_id;
		GLuint load_shader_program(const char* vertex_shader_file, const char* fragment_shader_file);
		bool read_file(const char* filename, string& string_out);
		bool did_shader_compile(GLuint id, GLuint type);
	public:
		ShaderProgram(const char* vertex_shader_file, const char* fragment_shader_file);
		unsigned int get_shader_id();
		void use_program();
		void delete_program();

		void  setUniform(const char* property_name, float property_value) { glUniform1f(glGetUniformLocation(this->shader_id, property_name), property_value); };
		void  setUniform(const char* property_name, int property_value) { glUniform1i(glGetUniformLocation(this->shader_id, property_name), property_value); };
		void  setUniform(const char* property_name, vec2 property_value) { glUniform2fv(glGetUniformLocation(this->shader_id, property_name), 1, glm::value_ptr(property_value)); };
		void  setUniform(const char* property_name, vec3 property_value) { glUniform3fv(glGetUniformLocation(this->shader_id, property_name), 1, glm::value_ptr(property_value)); };
		void  setUniform(const char* property_name, vec4 property_value) { glUniform4fv(glGetUniformLocation(this->shader_id, property_name), 1, glm::value_ptr(property_value)); };
		void  setUniform(const char* property_name, mat3 property_value) { glUniformMatrix3fv(glGetUniformLocation(this->shader_id, property_name), 1, GL_FALSE, glm::value_ptr(property_value)); };
		void  setUniform(const char* property_name, mat4 property_value) { glUniformMatrix4fv(glGetUniformLocation(this->shader_id, property_name), 1, GL_FALSE, glm::value_ptr(property_value)); };
	};
}

#endif
