#include <Engine/Bone.h>
#include <Rendering/Model.h>
#include <Core/Utility.h>

Bone::Bone(const std::string& name, int ID, const aiNodeAnim* channel)
	:
	m_Name(name),
	m_ID(ID),
	m_LocalTransform(1.0f)
{
	//garbage values interfear
	if (channel->mNumPositionKeys < 1000) {
		m_NumPositions = channel->mNumPositionKeys;

		for (int positionIndex = 0; positionIndex < m_NumPositions; ++positionIndex)
		{
			aiVector3D aiPosition = channel->mPositionKeys[positionIndex].mValue;
			float timeStamp = static_cast<float>((channel->mPositionKeys[positionIndex].mTime));
			KeyPosition data;
			data.position = Model::GetGLMVec(aiPosition);
			data.timeStamp = timeStamp;
			m_Positions.push_back(data);
		}
	}
	if (channel->mNumRotationKeys < 1000) {
		m_NumRotations = channel->mNumRotationKeys;
		for (int rotationIndex = 0; rotationIndex < m_NumRotations; ++rotationIndex)
		{
			aiQuaternion aiOrientation = channel->mRotationKeys[rotationIndex].mValue;
			float timeStamp = static_cast<float>((channel->mRotationKeys[rotationIndex].mTime));
			KeyRotation data;
			data.orientation = Model::GetGLMQuat(aiOrientation);//TODO(darius) move to core
			data.timeStamp = timeStamp;
			m_Rotations.push_back(data);
		}
	}
	if (channel->mNumScalingKeys < 1000) {
		m_NumScalings = channel->mNumScalingKeys;
		for (int keyIndex = 0; keyIndex < m_NumScalings; ++keyIndex)
		{
			aiVector3D scale = channel->mScalingKeys[keyIndex].mValue;
			float timeStamp = static_cast<float>((channel->mScalingKeys[keyIndex].mTime));
			KeyScale data;
			data.scale = Model::GetGLMVec(scale);
			data.timeStamp = timeStamp;
			m_Scales.push_back(data);
		}
	}
}

void Bone::Update(float animationTime)
{
	glm::mat4 translation = InterpolatePosition(animationTime);
	glm::mat4 rotation = InterpolateRotation(animationTime);
	glm::mat4 scale = InterpolateScaling(animationTime);
	m_LocalTransform = translation * rotation * scale;
}

int Bone::GetPositionIndex(float animationTime)
{
	for (int index = 0; index < m_NumPositions - 1; ++index)
	{
		if (animationTime < m_Positions[index + 1].timeStamp)
			return index;
	}
	assert(0);
	return 0;
}

int Bone::GetRotationIndex(float animationTime)
{
	for (int index = 0; index < m_NumRotations - 1; ++index)
	{
		if (animationTime < m_Rotations[index + 1].timeStamp)
			return index;
	}
	assert(0);
	return 0;
}

int Bone::GetScaleIndex(float animationTime)
{
	for (int index = 0; index < m_NumScalings - 1; ++index)
	{
		if (animationTime < m_Scales[index + 1].timeStamp)
			return index;
	}
	assert(0);
	return 0;
}

glm::mat4 Bone::InterpolatePosition(float animationTime)
{
	if (1 == m_NumPositions)
		return glm::translate(glm::mat4(1.0f), m_Positions[0].position);

	int p0Index = GetPositionIndex(animationTime);
	int p1Index = p0Index + 1;
	float scaleFactor = Math::getScaleFactor(m_Positions[p0Index].timeStamp,
		m_Positions[p1Index].timeStamp, animationTime);
	
	return Math::interpolatePosition(m_Positions[p0Index].position, m_Positions[p1Index].position, scaleFactor);
}

glm::mat4 Bone::InterpolateRotation(float animationTime)
{
	if (1 == m_NumRotations)
	{
		auto rotation = glm::normalize(m_Rotations[0].orientation);
		return glm::toMat4(rotation);
	}

	int p0Index = GetRotationIndex(animationTime);
	int p1Index = p0Index + 1;
	float scaleFactor = Math::getScaleFactor(m_Rotations[p0Index].timeStamp, m_Rotations[p1Index].timeStamp, animationTime);

	//TODO(darisu) compare slerp and nlerp
	glm::quat finalRotation = glm::normalize(glm::lerp(m_Rotations[p0Index].orientation, m_Rotations[p1Index].orientation, scaleFactor));
	finalRotation = glm::normalize(finalRotation);

	return glm::toMat4(finalRotation);
}

glm::mat4 Bone::InterpolateScaling(float animationTime)
{
	if (1 == m_NumScalings)
		return glm::scale(glm::mat4(1.0f), m_Scales[0].scale);

	int p0Index = GetScaleIndex(animationTime);
	int p1Index = p0Index + 1;
	float scaleFactor = Math::getScaleFactor(m_Scales[p0Index].timeStamp, m_Scales[p1Index].timeStamp, animationTime);

	glm::vec3 finalScale = glm::mix(m_Scales[p0Index].scale, m_Scales[p1Index].scale, scaleFactor);

	return glm::scale(glm::mat4(1.0f), finalScale);
}

std::ostream& operator<<(std::ostream& os, Bone& b)
{
	os << b.GetBoneName() << "\n";	

	os << "Positions: (size " << b.getPositions().size() << ")\n";
	os << "Positions: (official size " << b.m_NumPositions << ")\n";

	for(auto& p : b.getPositions())
	{
		os << p.position << "\n";
	}

	return os;
}
