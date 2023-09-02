#include "EBO.h"

void EBO::init() {
	OpenglWrapper::GenerateBuffers(&ebo_id);
}

void EBO::bind(size_t ind_len, GLvoid* indices) {
	OpenglWrapper::BindBuffer(ebo_id, GL_ELEMENT_ARRAY_BUFFER);
	OpenglWrapper::SetBufferData(ind_len, indices, GL_ELEMENT_ARRAY_BUFFER);
}

void EBO::deleteEBO() {
	glDeleteBuffers(1, (GLuint*)&ebo_id);
}
