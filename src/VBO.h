#pragma once

#include <OpenglWrapper.h>

#include <vector>

class VBO
{
public:
	void init();

	void bind(size_t arr_len, GLvoid* v);

	void deleteVBO();

	void bind_at(int i);

	void bind_only();

	void setVAO(int index, int size, GLenum type, GLboolean normalized, int stride, const void* ptr);
	
	void vboEnableVertexAttribArray(unsigned int id);

private:
	size_t vbo_id = 0;
};

