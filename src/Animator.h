#pragma once

#include <glm/glm.hpp>
#include <map>
#include <vector>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <Animation.h>
#include <Bone.h>

class Animator
{
public:
	Animator(Animation* animation);

	void UpdateAnimation(float dt);

	void PlayAnimation(Animation* pAnimation);

	void CalculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform);

	std::vector<glm::mat4> GetFinalBoneMatrices()
	{
		return m_FinalBoneMatrices;
	}

	float getCurrTime(){
		return m_CurrentTime;
	}

	float getDeltaTime(){
		return m_DeltaTime;
	}

private:
	std::vector<glm::mat4> m_FinalBoneMatrices;
	Animation* m_CurrentAnimation = nullptr;
	float m_CurrentTime = 0;
	float m_DeltaTime = 0;
};

std::ostream& operator<<(std::ostream& os, Animator& a);

