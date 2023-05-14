#pragma once
#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include <vector>

class VAO
{
public:
	void init() {
		glGenVertexArrays(1, &vao_id);
	}

	void bind() {
		glBindVertexArray(vao_id);
	}

	~VAO() {
		//glDeleteVertexArrays(1, &vao_id);
	}

private:
	unsigned int vao_id = 0;
};

