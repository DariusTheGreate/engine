#include "VAO.h"

void VAO::init() {
	OpenglWrapper::GenerateArrays(&vao_id);
}

void VAO::bind() {
	OpenglWrapper::BindArray(vao_id);
}

VAO::~VAO() {
	//glDeleteVertexArrays(1, &vao_id);
}
