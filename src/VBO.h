#pragma once

#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include <vector>

class VBO
{
public:
	void init() {
		glGenBuffers(1, &vbo_id);
	}

	void bind(size_t arr_len, GLvoid* v) {
		glBindBuffer(GL_ARRAY_BUFFER, vbo_id);
		glBufferData(GL_ARRAY_BUFFER, arr_len, v, GL_STATIC_DRAW);
	}

	void setVAO(int index, int size, GLenum type, GLboolean normalized, size_t stride, const void* ptr) {
		glVertexAttribPointer(index, size, type, normalized, stride, ptr);
	}
	
	void vboEnableVertexAttribArray(unsigned int id) {
		glEnableVertexAttribArray(id);
	}

	~VBO(){
		 //glDeleteBuffers(1, &vbo_id);
	}

private:
	unsigned int vbo_id = 0;
};

