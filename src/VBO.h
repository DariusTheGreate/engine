#pragma once

#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include <vector>

template<size_t ArrayLen>
class VBO
{
public:
	VBO() {
		glGenBuffers(1, &vbo_id);
	}

	void bind(float* v) {
		glBindBuffer(GL_ARRAY_BUFFER, vbo_id);
		glBufferData(GL_ARRAY_BUFFER, ArrayLen * sizeof(float), v, GL_STATIC_DRAW);
	}

	void setVAO(int index, int size, GLenum type, GLboolean normalized, size_t stride, const void* ptr) {
		glVertexAttribPointer(index, size, type, normalized, stride, ptr);
		glEnableVertexAttribArray(0);
	}

private:
	unsigned int vbo_id;
};
