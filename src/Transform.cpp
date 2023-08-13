#include <Transform.h>

Transform::Transform(glm::vec3 pos, glm::vec3 scl)
{
	setPosition(pos);
	setScale(scl);
}

Transform::Transform(const Transform& t) : matrix(t.matrix)
{
}

void Transform::setPosition(glm::vec3 pos)
{
	//position = pos;	
	//matrix[3] = glm::vec4(pos.x, pos.y, pos.z, 0);

    glm::mat4 mvp = glm::translate(matrix, pos);

    matrix = mvp;
}

glm::vec3 Transform::getPosition()
{
	//return position;
	return glm::vec3(matrix[3][0], matrix[3][1], matrix[3][2]);	
}

void Transform::addToPosition(glm::vec3 v)
{
	matrix[3][0] += v.x;
	matrix[3][1] += v.y;
	matrix[3][2] += v.z;
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
