#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/quaternion.hpp >

struct Transform
{
	Transform() = default;
	Transform(glm::vec3 pos, glm::vec3 scl) : position(pos), scale(scl) {}

	glm::mat4 get_quatmat()
	{
		glm::mat4 RotationMatrix = glm::toMat4(q);
		return RotationMatrix;
	}

	glm::quat q = glm::quat{ 0.0,0.0,0.0,1.0 };
	glm::vec3 position = {0,0,0};
	glm::vec3 scale = { 1,1,1 };
};

