#pragma once
#include <string>
#include <iostream>

#include <glad/glad.h> 
#include <GLFW/glfw3.h>

class Shader
{
public:
	using ShaderType = int;
	using ShaderProgram = unsigned int;

	Shader(const std::string& str, ShaderType type) : source(str) {

		shader = glCreateShader(type);
		auto ptr = source.c_str();
		glShaderSource(shader, 1, &ptr, NULL);
	}
	
	void compile() const {
		glCompileShader(shader);
		// check for shader compile errors
		int success;
		char infoLog[512];
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shader, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
		}
	}

	ShaderProgram link(const Shader& other) {
		ShaderProgram shaderProgram = glCreateProgram();
		glAttachShader(shaderProgram, shader);
		glAttachShader(shaderProgram, other.shader);
		glLinkProgram(shaderProgram);
		int success;
		char infoLog[512];
		glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
		}
		//glDeleteShader(vertexShader);
		//glDeleteShader(fragmentShader);
		return shaderProgram;
	}

	~Shader() {
		glDeleteShader(shader);
	}

private:
	std::string source;
	unsigned int shader;
};	

