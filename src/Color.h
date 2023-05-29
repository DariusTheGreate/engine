#pragma once

struct Color
{
	glm::vec3 color;	
	operator glm::vec3()
	{
		return color;
	}
};