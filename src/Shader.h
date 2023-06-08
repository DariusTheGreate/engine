#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

class Shader
{
public:
	using ShaderType = int;
	using ShaderProgram = unsigned int;

	Shader(){}

	Shader(const std::string& filepath, ShaderType type) {
		std::ifstream shaderFile;
		shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		
		try
		{
			shaderFile.open(filepath);
			std::stringstream vShaderStream;

			vShaderStream << shaderFile.rdbuf();

			shaderFile.close();

			source = vShaderStream.str();
		}
		catch (std::ifstream::failure e)
		{
			std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
		}
		
		shader = glCreateShader(type);
		auto ptr = source.c_str();
		glShaderSource(shader, 1, &ptr, NULL);
	}
	
	void compile() const {
		glCompileShader(shader);

		int success;
		const int err_len = 2048;
		char infoLog[err_len];
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shader, err_len, NULL, infoLog);
			std::cout << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
		}
	}

	ShaderProgram link(const Shader& other) {
		ShaderProgram shaderProgram = glCreateProgram();
		glAttachShader(shaderProgram, shader);
		glAttachShader(shaderProgram, other.shader);
		glLinkProgram(shaderProgram);
		int success;
		const int err_len = 2048;
		char infoLog[err_len];
		glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(shaderProgram, err_len, NULL, infoLog);
			std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
		}
		
		//not cool?
		//glDeleteShader(shader);
		//glDeleteShader(other.shader);

		ID = shaderProgram;
		
		return shaderProgram;
	}

	void setBool(const std::string& name, bool value) const
	{
		glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
	}

	void setInt(const std::string& name, int value) const
	{
		glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
	}

	void setFloat(const std::string& name, float value) const
	{
		glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
	}

	void setVec2(const std::string& name, const glm::vec2& value) const
	{
		glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
	}

	void setVec2(const std::string& name, float x, float y) const
	{
		glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
	}

	void setVec3(const std::string& name, const glm::vec3& value) const
	{
		glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
	}
	void setVec3(const std::string& name, float x, float y, float z) const
	{
		glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
	}

	void setVec4(const std::string& name, const glm::vec4& value) const
	{
		glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
	}

	void setVec4(const std::string& name, float x, float y, float z, float w) const
	{
		glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
	}

	void setMat2(const std::string& name, const glm::mat2& mat) const
	{
		glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}

	void setMat3(const std::string& name, const glm::mat3& mat) const
	{
		glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}

	void setMat4(const std::string& name, const glm::mat4& mat) const
	{
		glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}

	unsigned int getShader() const {
		return shader;
	}

	ShaderProgram getProgram() const 
	{
		return ID;
	}

	void use() 
    { 
        glUseProgram(ID); 
    }

	~Shader() {
		glDeleteShader(shader);
	}

private:
	std::string source;
	ShaderProgram ID;
	unsigned int shader;
};	

