#pragma once

#include <OpenglWrapper.h>

#include <vector>

class VAO
{
public:
	void init();

	void bind();

	void deleteVAO();

	int get();

private:
	int vao_id = 0;
};

