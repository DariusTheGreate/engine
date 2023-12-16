#pragma once

#include <Rendering/OpenglWrapper.h>

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

