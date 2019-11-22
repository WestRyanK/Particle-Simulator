#include "../Assets/ShaderProgram.h"

#include "../Dependencies/OpenGL.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

using CodeMonkeys::Engine::Assets::ShaderProgram;


bool ShaderProgram::read_file(const char* filename, std::string& string_out)
{
	std::ifstream file(filename);
	if (!file)
	{
		if (!file.is_open()) {
			std::cout << "Couldn't open file " << filename << " for reading." << std::endl;
		}
		return false;
	}

	std::stringstream ss;
	ss << file.rdbuf();
	string_out = ss.str();
	return true;
}

bool ShaderProgram::did_shader_compile(GLuint id, GLuint type)
{
	const int bufSize = 1024;
	char msgBuf[bufSize];
	GLint success;
	
	if (type == GL_PROGRAM)
	{
		glGetProgramiv(id, GL_LINK_STATUS, &success);
	}
	else
	{
		glGetShaderiv(id, GL_COMPILE_STATUS, &success);
	}
	if (!success)
	{
		if (type == GL_PROGRAM)
		{
			glGetProgramInfoLog(id, bufSize, NULL, msgBuf);
			std::cout << "Error linking program:\n";
		}
		else
		{
			glGetShaderInfoLog(id, bufSize, NULL, msgBuf);
			if (type == GL_VERTEX_SHADER)
			{
				std::cout << "Error compiling vertex shader:\n";
			}
			else if (type == GL_FRAGMENT_SHADER)
			{
				std::cout << "Error compiling fragment shader:\n";
			}
		}
		std::cout << msgBuf << std::endl;
		return false;
	}

	return true;
}

GLuint ShaderProgram::load_shader_program(const char* vertex_shader_file, const char* fragment_shader_file)
{
	std::string vertStrOut, fragStrOut;
	if (!ShaderProgram::read_file(vertex_shader_file, vertStrOut))
	{
		std::cout << "Error reading vertex shader file: " << vertex_shader_file << std::endl;
		return 0;
	}

	if (!read_file(fragment_shader_file, fragStrOut))
	{
		std::cout << "Error reading fragment shader file: " << fragment_shader_file << std::endl;
		return 0;
	}
	
	const char* vertShaderCode = vertStrOut.c_str();
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertShaderCode, NULL);
	glCompileShader(vertexShader);
	if (!ShaderProgram::did_shader_compile(vertexShader, GL_VERTEX_SHADER))
	{
		return 0;
	}

	const char* fragShaderCode = fragStrOut.c_str();
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragShaderCode, NULL);
	glCompileShader(fragmentShader);
	if (!ShaderProgram::did_shader_compile(fragmentShader, GL_FRAGMENT_SHADER))
	{
		return 0;
	}

	GLuint shaderID = glCreateProgram();
	glAttachShader(shaderID, vertexShader);
	glAttachShader(shaderID, fragmentShader);
	glLinkProgram(shaderID);
	if (!ShaderProgram::did_shader_compile(shaderID, GL_PROGRAM))
	{
		return 0;
	}

	// No longer needed at this point as they have already been linked into the shader program.
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	return shaderID;
}


ShaderProgram::ShaderProgram(const char* vertex_shader_file, const char* fragment_shader_file)
{
    this->shader_id = load_shader_program(vertex_shader_file, fragment_shader_file);
}

unsigned int ShaderProgram::get_shader_id()
{
	return this->shader_id;
}

void ShaderProgram::use_program()
{
	glUseProgram(this->shader_id);
}

void ShaderProgram::delete_program()
{
	glDeleteProgram(this->shader_id);
}
