#pragma once

#include <OpenglWrapper.h>

#include <vector>

class VBO
{
public:
	void init() {
        OpenglWrapper::GenerateBuffers(& vbo_id);
	}

	void bind(size_t arr_len, GLvoid* v) {
        OpenglWrapper::BindBuffer(vbo_id);
        OpenglWrapper::SetBufferData(arr_len, v);
	}

	void bind_at(int i)
	{
        OpenglWrapper::BindBuffer(i);
	}

	void bind_only()
	{
		OpenglWrapper::BindBuffer(vbo_id);	
	}

	void setVAO(int index, int size, GLenum type, GLboolean normalized, size_t stride, const void* ptr) {
        OpenglWrapper::AttributePointer(index, size, type, stride, ptr, normalized);
	}
	
	void vboEnableVertexAttribArray(unsigned int id) {
        OpenglWrapper::EnableAttribute(id);
	}

	~VBO(){
		 //glDeleteBuffers(1, &vbo_id);
	}

private:
	size_t vbo_id = 0;
};

