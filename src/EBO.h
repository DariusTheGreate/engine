#pragma once
#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include <vector>

class EBO
{
public:
	void init() {
		glGenBuffers(1, &ebo_id);
	}

	void bind(size_t ind_len, GLvoid* indices) {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_id);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, ind_len, indices, GL_STATIC_DRAW);
	}

	~EBO() {
		//glDeleteBuffers(1, &ebo_id);
	}

private:
	unsigned int ebo_id = 0;
};

