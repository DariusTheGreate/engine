#pragma once
#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include <vector>

class VAO
{
public:
	VAO() {
		glGenVertexArrays(1, &vao_id);
		glBindVertexArray(vao_id);
	}

	void bind() {
		glBindVertexArray(vao_id);
	}

private:
	unsigned int vao_id;
};

