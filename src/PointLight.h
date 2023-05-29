#pragma once

#include <glm/glm.hpp>

class PointLight{
public:
	PointLight() = default;
	PointLight(glm::vec3 position_in, glm::vec3 color_in, float intensity_in = 1.0f, float radius_in = 1.0f) : position(position_in), color(color_in), intensity(intensity_in), radius(radius_in) { }

	//TODO(darius) make it Tranform?
	glm::vec3 position = {0,0,0};
	glm::vec3 color = {1,1,1};

	float intensity = 1.0f;
	float radius = 1.0f;
};