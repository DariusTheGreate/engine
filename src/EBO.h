#pragma once
#include <vector>

#include <OpenglWrapper.h>

class EBO
{
public:
	void init();

	void bind(size_t ind_len, GLvoid* indices);

	~EBO();

private:
	size_t ebo_id = 0;
};

