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
	Transform(glm::vec3 pos, glm::vec3 scl, glm::quat rot);
	Transform(std::vector<float> mat_in);
	Transform(const Transform& t);

	void setPosition(glm::vec3 pos);

	glm::vec3 getPosition();

	void addToPosition(glm::vec3 v);

	void rotate(glm::quat q);

	void rotate(float angle_in_radians, glm::vec3 axis);

	void setScale(glm::vec3 scl);

	glm::vec3 getScale();

	std::vector<double> getMatrixVector();

	std::vector<float> matrixVector();

	glm::quat matrixQuaternion();

	int objectID = 0;

    glm::mat4 matrix = { 1.f, 0.f, 0.f, 0.f,
                        0.f, 1.f, 0.f, 0.f,
                        0.f, 0.f, 1.f, 0.f,
                        0.f, 0.f, 0.f, 1.f };	
};

