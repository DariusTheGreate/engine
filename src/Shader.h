#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

#include <OpenglWrapper.h>

class Shader
{
public:
	using ShaderType = int;
	using ShaderProgram = unsigned int;

	Shader(){}

	Shader(const std::string& filepath, ShaderType type) {
        //TODO(darius) make it filesystem
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
		
		shader = OpenglWrapper::CreateShader(type);
		auto ptr = source.c_str();
        OpenglWrapper::SetShaderSource(shader, ptr);
	}
	
	void compile() const {
		OpenglWrapper::CompileShader(shader);

        //TODO(darius) make it wrapper if needed
		int success;
		const int err_len = 2048;
		char infoLog[err_len];
        OpenglWrapper::GetShaderParam(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
            OpenglWrapper::GetShaderLog(shader, infoLog);
			std::cout << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
		}
	}

	ShaderProgram link(const Shader& other) {
		ShaderProgram shaderProgram = OpenglWrapper::CreateProgram();
        OpenglWrapper::AttachShader(shaderProgram, shader);
		OpenglWrapper::AttachShader(shaderProgram, other.shader);
        OpenglWrapper::LinkProgram(shaderProgram);
		int success;
		const int err_len = 2048;
		char infoLog[err_len];
        OpenglWrapper::GetProgramParam(shaderProgram, GL_LINK_STATUS, &success);
		if (!success) {
            OpenglWrapper::GetProgramLog(shaderProgram, infoLog);
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
        OpenglWrapper::SetShaderInt(ID, name.c_str(), (int)value);
	}

	void setInt(const std::string& name, int value) const
	{
        OpenglWrapper::SetShaderInt(ID, name.c_str(), (int)value);
	}

	void setFloat(const std::string& name, float value) const
	{
        OpenglWrapper::SetShaderFloat(ID, name.c_str(), value);
	}

	void setVec2(const std::string& name, const glm::vec2& value) const
	{
        OpenglWrapper::SetShaderVec2(ID, name.c_str(), value.x, value.y);
	}

	void setVec2(const std::string& name, float x, float y) const
	{
        OpenglWrapper::SetShaderVec2(ID, name.c_str(), x, y);
	}

	void setVec3(const std::string& name, const glm::vec3& value) const
	{
        OpenglWrapper::SetShaderVec3(ID, name.c_str(), value.x, value.y, value.z);
	}
	void setVec3(const std::string& name, float x, float y, float z) const
	{
        OpenglWrapper::SetShaderVec3(ID, name.c_str(), x, y, z);
	}

	void setVec4(const std::string& name, const glm::vec4& value) const
	{
        OpenglWrapper::SetShaderVec4(ID, name.c_str(), value.x, value.y, value.z, value.w);
	}

	void setVec4(const std::string& name, float x, float y, float z, float w) const
	{
        OpenglWrapper::SetShaderVec4(ID, name.c_str(), x, y, z, w);
	}

	void setMat2(const std::string& name, const glm::mat2& mat) const
	{
        OpenglWrapper::SetShaderMat2(ID, name.c_str(), &mat[0][0]);
	}

	void setMat3(const std::string& name, const glm::mat3& mat) const
	{
        OpenglWrapper::SetShaderMat3(ID, name.c_str(), &mat[0][0]);
	}

	void setMat4(const std::string& name, const glm::mat4& mat) const
	{
        OpenglWrapper::SetShaderMat4(ID, name.c_str(), &mat[0][0]);
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
        OpenglWrapper::UseProgram(ID); 
    }

	~Shader() {
        OpenglWrapper::DeleteShader(shader);
	}

private:
	std::string source;
	ShaderProgram ID;
	unsigned int shader;
};	

