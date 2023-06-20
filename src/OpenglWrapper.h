#pragma once

#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <Window.h>

class OpenglWrapper // : GraphicsWrapper
{
public:
    
    static void GenerateBuffers(size_t* ptr, size_t n = 1)
    {
		glGenBuffers(n, (GLuint*)ptr);
    }

    static void BindBuffer(size_t ptr, size_t type = GL_ARRAY_BUFFER)
    {
		glBindBuffer(type, (GLuint)ptr);
    }

    static void SetBufferData(size_t len, void* v, size_t type = GL_ARRAY_BUFFER, size_t draw_type = GL_STATIC_DRAW)
    {
		glBufferData(type, len, v, draw_type);
    }

    static void GenerateArrays(void* ptr, size_t n = 1)
    {
		glGenVertexArrays(n, (GLuint*)ptr);
    } 

    static void BindArray(size_t ID)
    {
		glBindVertexArray(ID);
    }

	static int CreateShader(int type)
	{
		return glCreateShader(type);
	}

	static size_t CreateProgram()
	{
		return glCreateProgram();
	}

	static void LinkProgram(size_t program)
	{
		glLinkProgram(program);
	}

	static void AttachShader(size_t program, int shader)
	{
		glAttachShader(program, shader);
	}

	static void SetShaderSource(int shader, const char* code)
	{
		glShaderSource(shader, 1, &code, NULL);
	}

	static void CompileShader(int shader)
	{
		glCompileShader(shader);
	}

	static void GetShaderParam(int shader, size_t param, int* ptr)
	{
		glGetShaderiv(shader, param, ptr);
	}

    static void GetProgramParam(int program, size_t param, int* ptr)
    {
		glGetProgramiv(program, param, ptr);
    }

    static void GetProgramLog(int program, char* buff, int len = 2048)
    {
        glGetProgramInfoLog(program, len, NULL, buff);
    }

	static int GetShaderCompileStatus(int shader)
	{
		int res = false;
		GetShaderParam(shader, GL_COMPILE_STATUS, &res);
		return res;
	}

	static void GetShaderLog(int shader, char* infoLog, int err_len = 2048)
	{
		glGetShaderInfoLog(shader, err_len, NULL, infoLog);
	}

	static void UseProgram(size_t shader)
	{
        glUseProgram(shader);
	}

	static void DeleteShader(size_t shader)
	{
        glDeleteProgram(shader);
	}

    static void SetShaderInt(size_t ID, const char* name, int value)
    {
		glUniform1i(glGetUniformLocation(ID, name), (int)value);
    }

    static void SetShaderFloat(size_t ID, const char* name, float value)
    {
		glUniform1f(glGetUniformLocation(ID, name), value);
    }

    static void SetShaderVec2(size_t ID, const char* name, float x, float y)
    {
		glUniform2f(glGetUniformLocation(ID, name), x, y);
    }

    static void SetShaderVec3(size_t ID, const char* name, float x, float y, float z)
    {
		glUniform3f(glGetUniformLocation(ID, name), x, y, z);
    }

    static void SetShaderVec4(size_t ID, const char* name, float x, float y, float z, float w)
    {
		glUniform4f(glGetUniformLocation(ID, name), x, y, z, w);
    }

    static void SetShaderMat2(size_t ID, const char* name, const float* m)
    {
		glUniformMatrix2fv(glGetUniformLocation(ID, name), 1, GL_FALSE, m);
    }

    static void SetShaderMat3(size_t ID, const char* name, const float* m)
    {
		glUniformMatrix3fv(glGetUniformLocation(ID, name), 1, GL_FALSE, m);
    }

    static void SetShaderMat4(size_t ID, const char* name, const float* m)
    {
		glUniformMatrix4fv(glGetUniformLocation(ID, name), 1, GL_FALSE, m);
    }

    static void EnableAttribute(size_t num)
    {
        glEnableVertexAttribArray(num);
    }

    static void AttributePointer(int index, int size, size_t type, size_t stride, const void* ptr, bool normalize = GL_FALSE)
    {
        glVertexAttribPointer(index, size, type, normalize, stride, ptr);
    }

    static void GenerateTextures(unsigned int* ID, size_t n = 1)
    {
        glGenTextures(n, (GLuint*)ID);
    }

    static void BindTexture(size_t texture, size_t target = GL_TEXTURE_2D)
    {
        glBindTexture(target, texture);
    }

    static void ImageTexture(size_t format, size_t width, size_t height, unsigned char* data, size_t target = GL_TEXTURE_2D, size_t type = GL_UNSIGNED_BYTE, int lvl = 0, int border = 0)
    {
        glTexImage2D(target, lvl, format, width, height, border, format, type, data);
    }

    static void GenerateMipmap()
    {
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    static void TextureParameter(size_t pname, int param, size_t target = GL_TEXTURE_2D)
    {
        glTexParameteri(target, pname, param);
    }

    static void ActivateTexture(size_t texture = GL_TEXTURE0)
    {
        glActiveTexture(texture);
    }

	static void DrawArrays(size_t count, size_t primitive = GL_TRIANGLES, size_t first = 0)
	{
        glDrawArrays(primitive, first, count);
	}

	static void DrawElements(size_t count, size_t primitive = GL_TRIANGLES, size_t type = GL_UNSIGNED_INT, const void* indices = nullptr)
	{
        glDrawElements(primitive, count, type, indices);
	}

    static void DrawInstances(size_t count, size_t amount, size_t primitives = GL_TRIANGLES, size_t type = GL_UNSIGNED_INT, const void* indices = nullptr)
    {
        glDrawElementsInstanced(GL_TRIANGLES, count, GL_UNSIGNED_INT, indices, amount);
    }

	static void UnbindVAO()
	{
        glBindVertexArray(0);
	}

	static void PollEvents()
	{
        glfwPollEvents();
	}

	static void GetWindowSize(Window* wind, int* W, int* H)
	{
        glfwGetFramebufferSize(wind->getWindow(), W, H);
	}

	static void SetWindow(size_t W, size_t H)
	{
        glViewport(0, 0, W, H);
	}

	static void EnableDepthTest()
	{
        glEnable(GL_DEPTH_TEST);
	}

	static void EnableMultisample()
	{
        glEnable(GL_MULTISAMPLE);  
	}

	static void EnableSRGB()
	{
        glEnable(GL_FRAMEBUFFER_SRGB); 
	}

	static void ClearScreen()
	{
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	}

	static void ClearBuffer()
	{
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	static void SwapBuffers(Window* wind)
	{
        glfwSwapBuffers(wind->getWindow());
	}

private:
	//TODO(darius) add info struct for displaying amount of draw calls and stuff
};

