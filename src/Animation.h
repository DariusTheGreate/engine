#pragma once

#include <vector>
#include <map>
#include <glm/glm.hpp>
#include <assimp/scene.h>
#include <Bone.h>
#include <functional>
#include <Model.h>

struct AssimpNodeData
{
	glm::mat4 transformation;
	std::string name;
	int childrenCount;
	std::vector<AssimpNodeData> children;
};

//TODO(darius) check this tutorial out https://ogldev.org/www/tutorial38/tutorial38.html
// ALSO MAKE UR OWN FKN ANIMATION FORMAT AND CONVERTORS, CAUSE THIS FCKN FBX DAE HUE FILES ARE SO FKIN BS THERE IS FKN SIMDJSON THAT FAST AS FUCK SO USE IT INSTEAD
class Animation
{
public:
	Animation(const std::string& animationPath, Model* model);

	Bone* FindBone(const std::string& name);
	
	float GetTicksPerSecond() 
	{ 
		return m_TicksPerSecond; 
	}

	float GetDuration() 
	{ 
		return m_Duration;
	}

	const AssimpNodeData& GetRootNode() 
	{
		return m_RootNode;
	}

	const std::map<std::string,BoneInfo>& GetBoneIDMap() 
	{ 
		return m_BoneInfoMap;
	}

private:
	void ReadMissingBones(const aiAnimation* animation, Model& model);

	void ReadHierarchyData(AssimpNodeData& dest, const aiNode* src);

private:
	float m_Duration;
	float m_TicksPerSecond;
	std::vector<Bone> m_Bones;
	AssimpNodeData m_RootNode;
	std::map<std::string, BoneInfo> m_BoneInfoMap;
};
