#include "VAO.h"

void VAO::init() {
	OpenglWrapper::GenerateArrays(&vao_id);
}

void VAO::bind() {
	OpenglWrapper::BindArray(vao_id);
}

void VAO::deleteVAO() {
	glDeleteVertexArrays(1, (GLuint*)&vao_id);
}

int VAO::get()
{
	return vao_id;
}

