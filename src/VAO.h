#pragma once

#include <OpenglWrapper.h>

#include <vector>

class VAO
{
public:
	void init();

	void bind();

	~VAO();

private:
	int vao_id = 0;
};

