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
		
		//not cool?
		glDeleteShader(shader);
		glDeleteShader(other.shader);
		
		return shaderProgram;
	}

	void changeProgramUniformState(unsigned int shaderProgram) {
		float timeValue = glfwGetTime();
		float greenValue = (sin(timeValue) / 2.0f) + 0.5f;
		int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");
		glUseProgram(shaderProgram);
		glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);
	}

	~Shader() {
		//glDeleteShader(shader);
	}

private:
	std::string source;
	unsigned int shader;
};	

