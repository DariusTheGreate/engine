#include "Shader.h"

Shader::Shader(const std::string& filepath_in, ShaderType type) : filepath(filepath_in) {
	shader = OpenglWrapper::CreateShader(type);
	reload();
}

void Shader::reload()
{
	load();
	auto ptr = source.c_str();
	OpenglWrapper::SetShaderSource(shader, ptr);
}

void Shader::load()
{
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
	catch (std::ifstream::failure& e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ: " << filepath << std::endl;
	}
}

void Shader::compile() const {
	OpenglWrapper::CompileShader(shader);

	//TODO(darius) make it wrapper if needed
	int success;
	const int err_len = 2048;
	char infoLog[err_len];

	OpenglWrapper::GetShaderParam(shader, GL_COMPILE_STATUS, &success);

	if (!success)
	{
		OpenglWrapper::GetShaderLog(shader, infoLog);
		std::cout << "ERROR::SHADER::COMPILATION_FAILED! SHADER_FILNAME: " << filepath << "\nINFO_LOG: " << infoLog << std::endl;
	}
}

ShaderProgram Shader::link(const Shader& other) {
	ShaderProgram shaderProgram = static_cast<unsigned int>(OpenglWrapper::CreateProgram());
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

void Shader::setBool(const std::string& name, bool value) const
{
	OpenglWrapper::SetShaderInt(ID, name.c_str(), (int)value);
}

void Shader::setInt(const std::string& name, int value) const
{
	OpenglWrapper::SetShaderInt(ID, name.c_str(), (int)value);
}

void Shader::setFloat(const std::string& name, float value) const
{
	OpenglWrapper::SetShaderFloat(ID, name.c_str(), value);
}

void Shader::setVec2(const std::string& name, const glm::vec2& value) const
{
	OpenglWrapper::SetShaderVec2(ID, name.c_str(), value.x, value.y);
}

void Shader::setVec2(const std::string& name, float x, float y) const
{
	OpenglWrapper::SetShaderVec2(ID, name.c_str(), x, y);
}

void Shader::setVec3(const std::string& name, const glm::vec3& value) const
{
	OpenglWrapper::SetShaderVec3(ID, name.c_str(), value.x, value.y, value.z);
}
void Shader::setVec3(const std::string& name, float x, float y, float z) const
{
	OpenglWrapper::SetShaderVec3(ID, name.c_str(), x, y, z);
}

void Shader::setVec4(const std::string& name, const glm::vec4& value) const
{
	OpenglWrapper::SetShaderVec4(ID, name.c_str(), value.x, value.y, value.z, value.w);
}

void Shader::setVec4(const std::string& name, float x, float y, float z, float w) const
{
	OpenglWrapper::SetShaderVec4(ID, name.c_str(), x, y, z, w);
}

void Shader::setMat2(const std::string& name, const glm::mat2& mat) const
{
	OpenglWrapper::SetShaderMat2(ID, name.c_str(), &mat[0][0]);
}

void Shader::setMat3(const std::string& name, const glm::mat3& mat) const
{
	OpenglWrapper::SetShaderMat3(ID, name.c_str(), &mat[0][0]);
}

void Shader::setMat4(const std::string& name, const glm::mat4& mat) const
{
	OpenglWrapper::SetShaderMat4(ID, name.c_str(), &mat[0][0]);
}

unsigned int Shader::getShader() const {
	return shader;
}

ShaderProgram Shader::getProgram() const
{
	return ID;
}

bool Shader::checkForSourceChanges()
{
	std::ifstream shaderFile;
	shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	std::string source2;

	try
	{
		shaderFile.open(filepath);
		std::stringstream vShaderStream;

		vShaderStream << shaderFile.rdbuf();

		shaderFile.close();

		source2 = vShaderStream.str();

		if (source.compare(source2)) 
		{
			std::cout << "NOTIFICATION::SHADER::FILE_WAS_CHANED_RELOADING_NEEDED: " << filepath << std::endl;
			return true;
		}
	}
	catch (std::ifstream::failure& e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ_AT_RELOADING: " << filepath << std::endl;
	}

	return false;
}

void Shader::use()
{
	OpenglWrapper::UseProgram(ID);
}

Shader::~Shader() {
	OpenglWrapper::DeleteShader(shader);
}

