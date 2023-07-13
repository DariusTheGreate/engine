#pragma once

#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <Window.h>

class OpenglWrapper // : GraphicsWrapper
{
public:
    
    static void GenerateBuffers(size_t* ptr, int n = 1)
    {
		glGenBuffers(n, (GLuint*)ptr);
    }

    static void BindBuffer(size_t ID, int type = GL_ARRAY_BUFFER)
    {
		glBindBuffer(type, (GLuint)ID);
    }

    static void SetBufferData(size_t len, void* v, int type = GL_ARRAY_BUFFER, int draw_type = GL_STATIC_DRAW)
    {
		glBufferData(type, len, v, draw_type);
    }

    static void GenerateFrameBuffers(size_t* ptr, int n = 1)
    {
        glGenFramebuffers(n, (GLuint*)ptr);
    }

    static void BindFrameBuffer(int ID)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, ID);
    }

    static void UnbindFrameBuffer()
    {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
    }

    static void ReadBuffer(int buff = GL_COLOR_ATTACHMENT0)
    {
        glReadBuffer(buff);
    }

    static void ReadZeroBuffer()
    {
        glReadBuffer(GL_NONE);
    }

    static void ReadPixels(int x, int y, void* pixel)
    {
        glReadPixels(x, y, 1, 1, GL_RGBA, GL_UNSIGNED_INT, pixel);
    }

    static void GenerateArrays(void* ptr, int n = 1)
    {
		glGenVertexArrays(n, (GLuint*)ptr);
    } 

    static void BindArray(int ID)
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

	static void LinkProgram(int program)
	{
		glLinkProgram(program);
	}

	static void AttachShader(int program, int shader)
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

	static void GetShaderParam(int shader, int param, int* ptr)
	{
		glGetShaderiv(shader, param, ptr);
	}

    static void GetProgramParam(int program, int param, int* ptr)
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

	static void UseProgram(int shader)
	{
        glUseProgram(shader);
	}

	static void DeleteShader(int shader)
	{
        glDeleteProgram(shader);
	}

    static void SetShaderInt(int ID, const char* name, int value)
    {
		glUniform1i(glGetUniformLocation(ID, name), (int)value);
    }

    static void SetShaderFloat(int ID, const char* name, float value)
    {
		glUniform1f(glGetUniformLocation(ID, name), value);
    }

    static void SetShaderVec2(int ID, const char* name, float x, float y)
    {
		glUniform2f(glGetUniformLocation(ID, name), x, y);
    }

    static void SetShaderVec3(int ID, const char* name, float x, float y, float z)
    {
		glUniform3f(glGetUniformLocation(ID, name), x, y, z);
    }

    static void SetShaderVec4(int ID, const char* name, float x, float y, float z, float w)
    {
		glUniform4f(glGetUniformLocation(ID, name), x, y, z, w);
    }

    static void SetShaderMat2(int ID, const char* name, const float* m)
    {
		glUniformMatrix2fv(glGetUniformLocation(ID, name), 1, GL_FALSE, m);
    }

    static void SetShaderMat3(int ID, const char* name, const float* m)
    {
		glUniformMatrix3fv(glGetUniformLocation(ID, name), 1, GL_FALSE, m);
    }

    static void SetShaderMat4(int ID, const char* name, const float* m)
    {
		glUniformMatrix4fv(glGetUniformLocation(ID, name), 1, GL_FALSE, m);
    }

    static void EnableAttribute(int num)
    {
        glEnableVertexAttribArray(num);
    }

    static void AttributePointer(int index, int size, int type, int stride, const void* ptr, bool normalize = GL_FALSE)
    {
        glVertexAttribPointer(index, size, type, normalize, stride, ptr);
    }

    static void GenerateTextures(size_t* ID, int n = 1)
    {
        glGenTextures(n, (GLuint*)ID);
    }

    static void BindTexture(int texture, int target = GL_TEXTURE_2D)
    {
        glBindTexture(target, texture);
    }

    static void UnbindTexture()
    {
        BindTexture(0);
    }

    static void ImageTexture(int format, int width, int height, unsigned char* data, int target = GL_TEXTURE_2D, int type = GL_UNSIGNED_BYTE, int lvl = 0, int border = 0)
    {
        glTexImage2D(target, lvl, format, width, height, border, format, type, data);
    }

    static void ImageFrameBuffer(int ID)
    {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, ID, 0);
    }

    static void GenerateMipmap()
    {
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    static void TextureParameter(int pname, int param, int target = GL_TEXTURE_2D)
    {
        glTexParameteri(target, pname, param);
    }

    static void ActivateTexture(int texture = GL_TEXTURE0)
    {
        glActiveTexture(texture);
    }

	static void DrawArrays(int count, int primitive = GL_TRIANGLES, int first = 0)
	{
        glDrawArrays(primitive, first, count);
	}

	static void DrawElements(int count, int primitive = GL_TRIANGLES, int type = GL_UNSIGNED_INT, const void* indices = nullptr)
	{
        glDrawElements(primitive, count, type, indices);
	}

    static void DrawInstances(int count, int amount, size_t primitives = GL_TRIANGLES, size_t type = GL_UNSIGNED_INT, const void* indices = nullptr)
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

	static void SetWindow(int W, int H)
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


