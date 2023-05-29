#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/quaternion.hpp >
#include <vector>

struct Transform
{
	Transform() = default;
	Transform(glm::vec3 pos, glm::vec3 scl) : position(pos), scale(scl) {}
	Transform(const Transform& t) : q(t.q), position(t.position), scale(t.scale) {}

	glm::mat4 get_quatmat()
	{
		glm::mat4 RotationMatrix = glm::toMat4(q);
		//RotationMatrix[3] = glm::vec4{ position.x,position.y,position.z,0  };
		return RotationMatrix;
	}

	void set_from_quatmat(glm::mat4 m)
	{
		q = m;
		position = m[3];
	}

	std::vector<double> get_matrix()
	{
		float x = q.x;
		float y = q.y;
		float z = q.z;
		float w = q.w;

		return std::vector<double>{ 1.0 - 2.0 * y * y - 2.0 * z * z, 2.0 * x * y - 2.0 * w * z, 2.0 * x * z + 2.0 * w * y, 0.0,
			2.0 * x * y + 2.0 * w * z, 1.0 - 2.0 * x * x - 2.0 * z * z, 2.0 * y * z - 2.0 * w * x, 0.0,
			2.0 * x * z - 2.0 * w * y, 2.0 * y * z + 2.0 * w * x, 1.0 - 2.0 * x * x - 2.0 * y * y, 0.0,
			0.0, 0.0, 0.0, 1.0 };
	}

	glm::quat q = glm::quat{ 0.0,0.0,0.0,1.0 };
	glm::vec3 position = {0,0,0};
	glm::vec3 scale = { 1,1,1 };
};

