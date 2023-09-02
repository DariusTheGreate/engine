#pragma once
#include <vector>

#include <OpenglWrapper.h>

class EBO
{
public:
	void init();

	void bind(size_t ind_len, GLvoid* indices);

	void deleteEBO();

private:
	size_t ebo_id = 0;
};

