#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/quaternion.hpp >
#include <vector>

// https://www.brainvoyager.com/bv/doc/UsersGuide/CoordsAndTransforms/SpatialTransformationMatrices.html
struct Transform
{
	Transform() = default;
	Transform(glm::vec3 pos, glm::vec3 scl);
	Transform(const Transform& t);

	glm::mat4 get_quatmat();

	void set_from_quatmat(glm::mat4 m);
	void setPosition(glm::mat4 m);
	void setScale(glm::mat4 m);

	void set_scale(glm::vec3 scl);

	std::vector<double> get_matrix();

	void rotateBy(float angleDegree, glm::vec3 axis);

	glm::quat q = glm::quat{ 0.0,0.0,0.0,1.0 };
	glm::vec3 position = {0,0,0};
	glm::vec3 scale = { 1,1,1 };

	int objectID = 0;
};

