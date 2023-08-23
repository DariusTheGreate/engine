#include <Transform.h>

Transform::Transform(glm::vec3 pos, glm::vec3 scl)
{
	//TODO(darius) figure out order of operations.
	setPosition(pos);
	setScale(scl);
}

Transform::Transform(glm::vec3 pos, glm::vec3 scl, glm::quat rot)
{
	setPosition(pos);
	rotate(rot);
	setScale(scl);
}

Transform::Transform(const Transform& t) : matrix(t.matrix)
{
}

Transform::Transform(std::vector<float> mat_in)
{
	int c = 0;//NOTE(darius) make it i * j

	for(int i = 0; i < 4; ++i)
	{
		for(int j = 0; j < 4; ++j)
		{
			matrix[i][j] = mat_in[c++];
		}
	}
}

Transform::Transform(glm::mat4 mat_in) : matrix(mat_in)
{
}

void Transform::setPosition(glm::vec3 pos)
{
	matrix[3][0] = pos.x;
	matrix[3][1] = pos.y;
	matrix[3][2] = pos.z;
}

void Transform::translatePosition(glm::vec3 pos)
{
    glm::mat4 mvp = glm::translate(matrix, pos);
    matrix = mvp;
}

glm::vec3 Transform::getPosition()
{
	//return position;
	return glm::vec3(matrix[3][0], matrix[3][1], matrix[3][2]);	
}

//TODO(darius) test it
void Transform::addToPosition(glm::vec3 v)
{
	/*matrix[3][0] += v.x;
	matrix[3][1] += v.y;
	matrix[3][2] += v.z;
	*/

	glm::vec3 newPos = getPosition() + v;
	setPosition(newPos);
}

void Transform::rotate(glm::quat q)
{
	glm::mat4 RotationMatrix = glm::toMat4(q);
	matrix *= RotationMatrix;
}

void Transform::rotate(float angle_in_radians, glm::vec3 axis)
{
	glm::mat4 m = glm::rotate(matrix, angle_in_radians, axis);	
	matrix = m;
}

void Transform::setScale(glm::vec3 scl)
{
	//matrix[0][0] = scl.x;
	//matrix[1][1] = scl.y;
	//matrix[2][2] = scl.z;
	glm::mat4 mvp = glm::scale(matrix, scl);
	matrix = mvp;
}

glm::vec3 Transform::getScale()
{
	return glm::vec3(matrix[0][0], matrix[1][1], matrix[2][2]);	
}

std::vector<double> Transform::getMatrixVector()
{
	glm::quat q = matrix;
	float x = q.x;
	float y = q.y;
	float z = q.z;
	float w = q.w;

	return std::vector<double>{ 1.0 - 2.0 * y * y - 2.0 * z * z, 2.0 * x * y - 2.0 * w * z, 2.0 * x * z + 2.0 * w * y, 0.0,
		2.0 * x * y + 2.0 * w * z, 1.0 - 2.0 * x * x - 2.0 * z * z, 2.0 * y * z - 2.0 * w * x, 0.0,
		2.0 * x * z - 2.0 * w * y, 2.0 * y * z + 2.0 * w * x, 1.0 - 2.0 * x * x - 2.0 * y * y, 0.0,
		0.0, 0.0, 0.0, 1.0 };
}

std::vector<float> Transform::matrixVector()
{
	std::vector<float> res;	

	for(int i = 0; i < 4; ++i)
	{
		for(int j = 0; j < 4; ++j)
		{
			res.push_back(matrix[i][j]);	
		}	
	}

	return res;
}

glm::quat Transform::matrixQuaternion()
{
	return matrix;	
}

void Transform::setMatrix(glm::mat4 matrix_in)
{
	matrix = matrix_in;
}

