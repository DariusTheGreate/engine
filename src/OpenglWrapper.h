#pragma once
#include <Window.h>

class OpenglWrapper // : GraphicsWrapper
{
public:
    
    static void GenerateBuffers(size_t* ptr, int n = 1);

	static void BindBuffer(size_t ID, int type = GL_ARRAY_BUFFER);

	static void SetBufferData(size_t len, void* v, int type = GL_ARRAY_BUFFER, int draw_type = GL_STATIC_DRAW);

	static void GenerateFrameBuffers(size_t* ptr, int n = 1);

	static void BindFrameBuffer(int ID);

	static void UnbindFrameBuffer();

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

	static void GenerateTextures(size_t* ID, int n = 1);

	static void BindTexture(int texture, int target = GL_TEXTURE_2D);

	static void UnbindTexture();

	static void ImageTexture(int format, int width, int height, unsigned char* data, int target = GL_TEXTURE_2D, int type = GL_UNSIGNED_BYTE, int lvl = 0, int border = 0);

	static void ImageFrameBuffer(int ID);

	static void GenerateMipmap();

	static void TextureParameter(int pname, int param, int target = GL_TEXTURE_2D);

	static void ActivateTexture(int texture = GL_TEXTURE0);

	static void DrawArrays(int count, int primitive = GL_TRIANGLES, int first = 0);

	static void DrawElements(int count, int primitive = GL_TRIANGLES, int type = GL_UNSIGNED_INT, const void* indices = nullptr);

	static void DrawInstances(int count, int amount, size_t primitives = GL_TRIANGLES, size_t type = GL_UNSIGNED_INT, const void* indices = nullptr);

	static void UnbindVAO();

	static void PollEvents();

	static void GetWindowSize(Window* wind, int* W, int* H);

	static void SetWindow(int W, int H);

	static void EnableDepthTest();

	static void EnableMultisample();

	static void EnableSRGB();

	static void ClearScreen();

	static void ClearBuffer();

	static void SwapBuffers(Window* wind);

private:
	//TODO(darius) add info struct for displaying amount of draw calls and stuff
};


