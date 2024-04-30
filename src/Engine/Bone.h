#pragma once

/* Container for bone data */

#include <vector>
#include <assimp/scene.h>
#include <list>
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

struct KeyPosition
{
	glm::vec3 position;
	float timeStamp;
};

struct KeyRotation
{
	glm::quat orientation;
	float timeStamp;
};

struct KeyScale
{
	glm::vec3 scale;
	float timeStamp;
};

class Bone
{
public:
	Bone(const std::string& name, int ID, const aiNodeAnim* channel);

	void Update(float animationTime);

	glm::mat4 GetLocalTransform() 
	{ 
		return m_LocalTransform; 
	}

	std::string GetBoneName() const 
	{
		return m_Name; 
	}

	std::vector<KeyPosition>& getPositions()
	{
		return m_Positions;
	}

	int GetBoneID() 
	{ 
		return m_ID; 
	}
	
	int GetPositionIndex(float animationTime);

	int GetRotationIndex(float animationTime);

	int GetScaleIndex(float animationTime);

private:
	glm::mat4 InterpolatePosition(float animationTime);

	glm::mat4 InterpolateRotation(float animationTime);

	glm::mat4 InterpolateScaling(float animationTime);

public:
	std::vector<KeyPosition> m_Positions;
	std::vector<KeyRotation> m_Rotations;
	std::vector<KeyScale> m_Scales;
	int m_NumPositions = 0;
	int m_NumRotations = 0;
	int m_NumScalings = 0;

	glm::mat4 m_LocalTransform;
	std::string m_Name;
	int m_ID;
};

std::ostream& operator<<(std::ostream& os, Bone& a);

