#include "VBO.h"

void VBO::init() {
	OpenglWrapper::GenerateBuffers(&vbo_id);
}

void VBO::bind(size_t arr_len, GLvoid* v) {
	OpenglWrapper::BindBuffer(vbo_id);
	OpenglWrapper::SetBufferData(arr_len, v);
}

void VBO::bind_at(int i)
{
	OpenglWrapper::BindBuffer(i);
}

void VBO::bind_only()
{
	OpenglWrapper::BindBuffer(vbo_id);
}

void VBO::setVAO(int index, int size, GLenum type, GLboolean normalized, int stride, const void* ptr) {
	OpenglWrapper::AttributePointer(index, size, type, stride, ptr, normalized);
}

void VBO::vboEnableVertexAttribArray(unsigned int id) {
	OpenglWrapper::EnableAttribute(id);
}

void VBO::deleteVBO() {
	glDeleteBuffers(1, (GLuint*)&vbo_id);
}
