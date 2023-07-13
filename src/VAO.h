#pragma once

#include <OpenglWrapper.h>

#include <vector>

class VAO
{
public:
	void init() {
        OpenglWrapper::GenerateArrays(&vao_id);
	}

	void bind() {
        OpenglWrapper::BindArray(vao_id);
	}

	~VAO() {
		//glDeleteVertexArrays(1, &vao_id);
	}

private:
	int vao_id = 0;
};

