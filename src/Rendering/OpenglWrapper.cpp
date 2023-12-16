#include <Rendering/OpenglWrapper.h>
#include <Rendering/Shader.h>
#include <glad/glad.h> 
#include <GLFW/glfw3.h>

void OpenglWrapper::GenerateBuffers(size_t* ptr, int n)
{
    glGenBuffers(n, (GLuint*)ptr);
}

void OpenglWrapper::BindBuffer(size_t ID, int type)
{
    glBindBuffer(type, (GLuint)ID);
}

void OpenglWrapper::SetBufferData(size_t len, void* v, int type, int draw_type)
{
    glBufferData(type, len, v, draw_type);
}

void OpenglWrapper::GenerateFrameBuffers(size_t* ptr, int n)
{
    glGenFramebuffers(n, (GLuint*)ptr);
}

void OpenglWrapper::BindFrameBuffer(int ID, GLenum target)
{
    glBindFramebuffer(target, ID);
}

void OpenglWrapper::UnbindFrameBuffer(GLenum target)
{
    glBindFramebuffer(target, 0);
}

void OpenglWrapper::ReadBuffer(int buff)
{
    glReadBuffer(buff);
}

void OpenglWrapper::ReadZeroBuffer()
{
    glReadBuffer(GL_NONE);
}

void OpenglWrapper::ReadPixels(int x, int y, void* pixel)
{
    glReadPixels(x, y, 1, 1, GL_RGBA, GL_FLOAT, pixel);
}

void OpenglWrapper::GenerateArrays(void* ptr, int n)
{
    glGenVertexArrays(n, (GLuint*)ptr);
}

void OpenglWrapper::BindArray(int ID)
{
    glBindVertexArray(ID);
}

int OpenglWrapper::CreateShader(int type)
{
    return glCreateShader(type);
}

size_t OpenglWrapper::CreateProgram()
{
    return glCreateProgram();
}

void OpenglWrapper::LinkProgram(int program)
{
    glLinkProgram(program);
}

void OpenglWrapper::AttachShader(int program, int shader)
{
    glAttachShader(program, shader);
}

void OpenglWrapper::SetShaderSource(int shader, const char* code)
{
    glShaderSource(shader, 1, &code, NULL);
}

void OpenglWrapper::CompileShader(int shader)
{
    glCompileShader(shader);
}

void OpenglWrapper::GetShaderParam(int shader, int param, int* ptr)
{
    glGetShaderiv(shader, param, ptr);
}

void OpenglWrapper::GetProgramParam(int program, int param, int* ptr)
{
    glGetProgramiv(program, param, ptr);
}

void OpenglWrapper::GetProgramLog(int program, char* buff, int len)
{
    glGetProgramInfoLog(program, len, NULL, buff);
}

int OpenglWrapper::GetShaderCompileStatus(int shader)
{
    int res = false;
    GetShaderParam(shader, GL_COMPILE_STATUS, &res);
    return res;
}

void OpenglWrapper::GetShaderLog(int shader, char* infoLog, int err_len)
{
    glGetShaderInfoLog(shader, err_len, NULL, infoLog);
}

void OpenglWrapper::UseProgram(int shader)
{
    glUseProgram(shader);
}

void OpenglWrapper::UseProgram(const Shader& shader)
{
    glUseProgram(shader.getProgram());
}

void OpenglWrapper::DeleteShader(int shader)
{
    glDeleteProgram(shader);
}

void OpenglWrapper::SetShaderInt(int ID, const char* name, int value)
{
    glUniform1i(glGetUniformLocation(ID, name), (int)value);
}

void OpenglWrapper::SetShaderFloat(int ID, const char* name, float value)
{
    glUniform1f(glGetUniformLocation(ID, name), value);
}

void OpenglWrapper::SetShaderVec2(int ID, const char* name, float x, float y)
{
    glUniform2f(glGetUniformLocation(ID, name), x, y);
}

void OpenglWrapper::SetShaderVec3(int ID, const char* name, float x, float y, float z)
{
    glUniform3f(glGetUniformLocation(ID, name), x, y, z);
}

void OpenglWrapper::SetShaderVec4(int ID, const char* name, float x, float y, float z, float w)
{
    glUniform4f(glGetUniformLocation(ID, name), x, y, z, w);
}

void OpenglWrapper::SetShaderMat2(int ID, const char* name, const float* m)
{
    glUniformMatrix2fv(glGetUniformLocation(ID, name), 1, GL_FALSE, m);
}

void OpenglWrapper::SetShaderMat3(int ID, const char* name, const float* m)
{
    glUniformMatrix3fv(glGetUniformLocation(ID, name), 1, GL_FALSE, m);
}

void OpenglWrapper::SetShaderMat4(int ID, const char* name, const float* m)
{
    glUniformMatrix4fv(glGetUniformLocation(ID, name), 1, GL_FALSE, m);
}

void OpenglWrapper::EnableAttribute(int num)
{
    glEnableVertexAttribArray(num);
}

void OpenglWrapper::AttributePointer(int index, int size, int type, int stride, const void* ptr, bool normalize)
{
    glVertexAttribPointer(index, size, type, normalize, stride, ptr);
}

void OpenglWrapper::GenerateTextures(unsigned int* ID, int n)
{
    glGenTextures(n, (GLuint*)ID);
}

void OpenglWrapper::BindTexture(int texture, int target)
{
    glBindTexture(target, texture);
}

void OpenglWrapper::UnbindTexture()
{
    BindTexture(0);
}

void OpenglWrapper::ImageTexture(int format, int width, int height, unsigned char* data, int target, int type, int lvl, int border)
{
    glTexImage2D(target, lvl, format, width, height, border, format, type, data);
}

void OpenglWrapper::ImageMultisampleTexture(int format, int width, int height, unsigned int samples, int target)
{
    glTexImage2DMultisample(target, samples, format, width, height, GL_TRUE);
}

void OpenglWrapper::ImageFrameBuffer(int ID, GLenum attach, GLenum target)
{
    glFramebufferTexture2D(GL_FRAMEBUFFER, attach, target, ID, 0);
}

void OpenglWrapper::GenerateMipmap()
{
    glGenerateMipmap(GL_TEXTURE_2D);
}

void OpenglWrapper::TextureParameter(int pname, int param, int target)
{
    glTexParameteri(target, pname, param);
}

void OpenglWrapper::ActivateTexture(int texture)
{
    glActiveTexture(texture);
}

void OpenglWrapper::DrawArrays(int count, int primitive, int first)
{
    glDrawArrays(primitive, first, count);
}

void OpenglWrapper::DrawElements(int count, int primitive, int type, const void* indices)
{
    glDrawElements(primitive, count, type, indices);
}

void OpenglWrapper::DrawInstances(int count, int amount, size_t primitives, size_t type, const void* indices)
{
    glDrawElementsInstanced(GL_TRIANGLES, count, GL_UNSIGNED_INT, indices, amount);
}

void OpenglWrapper::UnbindVAO()
{
    glBindVertexArray(0);
}

void OpenglWrapper::PollEvents()
{
    glfwPollEvents();
}

void OpenglWrapper::GetWindowSize(Window* wind, int* W, int* H)
{
    glfwGetFramebufferSize(wind->getWindow(), W, H);
}

void OpenglWrapper::SetWindowSize(Window* wind, int W, int H)
{
    glfwSetWindowSize(wind->getWindow(), W, H);
}

void OpenglWrapper::SetWindow(int W, int H)
{
    glViewport(0, 0, W, H);
}

void OpenglWrapper::EnableDepthTest()
{
    glEnable(GL_DEPTH_TEST);
}

void OpenglWrapper::DisableDepthTest()
{
	glDisable(GL_DEPTH_TEST);
}

void OpenglWrapper::EnableBlending()
{
    glEnable(GL_BLEND);
}

void OpenglWrapper::DisableBlending()
{
    glDisable(GL_BLEND);
}

void OpenglWrapper::EnableMultisample()
{
    glEnable(GL_MULTISAMPLE);
}

void OpenglWrapper::DisableMultisample()
{
	glClear(GL_MULTISAMPLE);
}

void OpenglWrapper::EnableSRGB()
{
    glEnable(GL_FRAMEBUFFER_SRGB);
}

void OpenglWrapper::ClearScreen(glm::vec3 color)
{
    glClearColor(color.x, color.y, color.z, 1.0f);
}

void OpenglWrapper::ClearBuffer()
{
    glClear(GL_COLOR_BUFFER_BIT);
}

void OpenglWrapper::ClearDepthBuffer()
{

    glClear(GL_DEPTH_BUFFER_BIT);
}

void OpenglWrapper::SwapBuffers(Window* wind)
{
    glfwSwapBuffers(wind->getWindow());
}

void OpenglWrapper::CullFaces()
{
    //NOTE do we need specify which faces?
    glEnable(GL_CULL_FACE);
}

char* OpenglWrapper::GetString(GLenum value)
{
    return (char*)glGetString(value);
}

