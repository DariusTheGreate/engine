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
	Transform(glm::mat4 mat_in);

	Transform(const Transform& t);

	//Transform& operator =(Transform swap);

	void setPosition(glm::vec3 pos);

	void translatePosition(glm::vec3 pos);

	glm::vec3 getPosition();

	glm::vec3 getRight() const
	{
		return matrix[0];
	}

	glm::vec3 getUp() const
	{
		return matrix[1];
	}

	glm::vec3 getBackward() const
	{
		return matrix[2];
	}

	glm::vec3 getForward() const
	{
		return -matrix[2];
	}

	void addToPosition(glm::vec3 v);

	void rotate(const glm::quat& q);

	void rotate(float angle_in_radians, glm::vec3 axis);

	void resetRotation();

	void setScale(glm::vec3 scl);

	glm::vec3 getScale();

	std::vector<double> getMatrixVector();

	std::vector<float> matrixVector();

	glm::quat matrixQuaternion();

	void setMatrix(glm::mat4 matrix_in);

	//NOTE(darius) it shouldnt be here (?) but its best if it here.
	int objectID = 0;

    glm::mat4 matrix = { 1.f, 0.f, 0.f, 0.f,
                        0.f, 1.f, 0.f, 0.f,
                        0.f, 0.f, 1.f, 0.f,
                        0.f, 0.f, 0.f, 1.f };	
};

