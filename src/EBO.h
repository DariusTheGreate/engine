#pragma once
#include <vector>

#include <OpenglWrapper.h>

class EBO
{
public:
	void init() {
        OpenglWrapper::GenerateBuffers(&ebo_id);
	}

	void bind(size_t ind_len, GLvoid* indices) {
        OpenglWrapper::BindBuffer(ebo_id,GL_ELEMENT_ARRAY_BUFFER);
        OpenglWrapper::SetBufferData(ind_len, indices, GL_ELEMENT_ARRAY_BUFFER);
	}

	~EBO() {
		//glDeleteBuffers(1, &ebo_id);
	}

private:
	size_t ebo_id = 0;
};

