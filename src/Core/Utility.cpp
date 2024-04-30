#include <Core/Utility.h>

glm::mat4 Math::interpolatePosition(const glm::vec3& posa, const glm::vec3& posb, float scaleFactor)
{
	//TODO(darius) compare with std::lerp, check which is better
	glm::vec3 finalPosition = glm::mix(posa, posb, scaleFactor);
	return glm::translate(glm::mat4(1.0f), finalPosition);
}

float Math::getScaleFactor(float t1, float t2, float tStamp)
{
	float scale = 0.0f;
	float passedSegment = tStamp - t1;
	float totalSegment = t2 - t1;
	scale = passedSegment / totalSegment;
	return scale;
}

