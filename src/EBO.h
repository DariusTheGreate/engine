#pragma once
#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include <vector>

template<size_t Indices_Len>
class EBO
{
public:
	explicit EBO() {
		glGenBuffers(1, &ebo_id);
	}

	void bind(unsigned int* indices) {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_id);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, Indices_Len, indices, GL_STATIC_DRAW);

		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_id);
	}

private:
	unsigned int ebo_id;
};

