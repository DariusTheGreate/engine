#pragma once
#include <Window.h>
#include <glm/glm.hpp>

class Shader;

class OpenglWrapper // : GraphicsWrapper
{
public:
	//TODO(darius) fix this
    
    static void GenerateBuffers(size_t* ptr, int n = 1);

	static void BindBuffer(size_t ID, int type = GL_ARRAY_BUFFER);

	static void SetBufferData(size_t len, void* v, int type = GL_ARRAY_BUFFER, int draw_type = GL_STATIC_DRAW);

	static void GenerateFrameBuffers(size_t* ptr, int n = 1);

	static void BindFrameBuffer(int ID, GLenum target = GL_FRAMEBUFFER);

	static void UnbindFrameBuffer(GLenum target = GL_READ_FRAMEBUFFER);

	static void ReadBuffer(int buff = GL_COLOR_ATTACHMENT0);

	static void ReadZeroBuffer();

	static void ReadPixels(int x, int y, void* pixel);

	static void GenerateArrays(void* ptr, int n = 1);

	static void BindArray(int ID);

	static int CreateShader(int type);

	static size_t CreateProgram();

	static void LinkProgram(int program);

	static void AttachShader(int program, int shader);

	static void SetShaderSource(int shader, const char* code);

	static void CompileShader(int shader);

	static void GetShaderParam(int shader, int param, int* ptr);

	static void GetProgramParam(int program, int param, int* ptr);

	static void GetProgramLog(int program, char* buff, int len = 2048);

	static int GetShaderCompileStatus(int shader);

	static void GetShaderLog(int shader, char* infoLog, int err_len = 2048);

	static void UseProgram(int shader);

	static void UseProgram(const Shader& shader);

	static void DeleteShader(int shader);

	static void SetShaderInt(int ID, const char* name, int value);

	static void SetShaderFloat(int ID, const char* name, float value);

	static void SetShaderVec2(int ID, const char* name, float x, float y);

	static void SetShaderVec3(int ID, const char* name, float x, float y, float z);

	static void SetShaderVec4(int ID, const char* name, float x, float y, float z, float w);

	static void SetShaderMat2(int ID, const char* name, const float* m);

	static void SetShaderMat3(int ID, const char* name, const float* m);

	static void SetShaderMat4(int ID, const char* name, const float* m);

	static void EnableAttribute(int num);

	static void AttributePointer(int index, int size, int type, int stride, const void* ptr, bool normalize = GL_FALSE);

	static void GenerateTextures(unsigned int* ID, int n = 1);

	static void BindTexture(int texture, int target = GL_TEXTURE_2D);

	static void UnbindTexture();

	static void ImageTexture(int format, int width, int height, unsigned char* data, int target = GL_TEXTURE_2D, int type = GL_UNSIGNED_BYTE, int lvl = 0, int border = 0);

	static void ImageMultisampleTexture(int format, int width, int height, unsigned int samples, int target = GL_TEXTURE_2D_MULTISAMPLE);

	static void ImageFrameBuffer(int ID, GLenum attachment = GL_DEPTH_ATTACHMENT, GLenum target = GL_TEXTURE_2D);

	static void GenerateMipmap();

	static void TextureParameter(int pname, int param, int target = GL_TEXTURE_2D);

	static void ActivateTexture(int texture = GL_TEXTURE0);

	static void DrawArrays(int count, int primitive = GL_TRIANGLES, int first = 0);

	static void DrawElements(int count, int primitive = GL_TRIANGLES, int type = GL_UNSIGNED_INT, const void* indices = nullptr);

	static void DrawInstances(int count, int amount, size_t primitives = GL_TRIANGLES, size_t type = GL_UNSIGNED_INT, const void* indices = nullptr);

	static void UnbindVAO();

	static void PollEvents();

	//NOTE(darius) its framebuffer size!
	static void GetWindowSize(Window* wind, int* W, int* H);

	static void SetWindowSize(Window* wind, int W, int H);

	static void SetWindow(int W, int H);

	static void EnableDepthTest();

	static void DisableDepthTest();

	static void EnableBlending();

	static void DisableBlending();

	static void EnableMultisample();

	static void DisableMultisample();

	static void EnableSRGB();

	static void ClearScreen(glm::vec3 color = {0,0,0});

	static void ClearBuffer();

	static void ClearDepthBuffer();

	static void SwapBuffers(Window* wind);

	static void CullFaces();

	static char* GetString(GLenum value);

private:
	//TODO(darius) add info struct for displaying amount of draw calls and stuff
};


