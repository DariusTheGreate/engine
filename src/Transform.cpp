#include <Transform.h>

Transform::Transform(glm::vec3 pos, glm::vec3 scl) : position(pos), scale(scl) {}
Transform::Transform(const Transform& t) : q(t.q), position(t.position), scale(t.scale) {}

glm::mat4 Transform::get_quatmat()
{
	glm::mat4 RotationMatrix = glm::toMat4(q);
	//RotationMatrix[3] = glm::vec4{ position.x,position.y,position.z,0  };
	return RotationMatrix;
}

void Transform::set_from_quatmat(glm::mat4 m)
{
	q = m;
	//position = m[3];

	//scale.x = m[0][0];
	//scale.y = m[1][1];
	//scale.z = m[2][2];
}

void Transform::setPosition(glm::mat4 m)
{
	position = m[3]; 
}

void Transform::setScale(glm::mat4 m)
{
	scale.x = m[0][0];
	scale.y = m[1][1];
	scale.z = m[2][2];
}

void Transform::set_scale(glm::vec3 scl)
{
	scale = scl;
}

std::vector<double> Transform::get_matrix()
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

//NOTE(darius) in progress
void Transform::rotateBy(float angleDegree, glm::vec3 axis)
{
	glm::mat4 mat = get_quatmat();
	glm::vec3 pos = mat[3];
	glm::rotate(mat, glm::radians(angleDegree), axis);
	mat[3][0] = pos[0];
	mat[3][1] = pos[1];
	mat[3][2] = pos[2];
	set_from_quatmat(mat);
}

