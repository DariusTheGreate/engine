#include <Animation.h>
#include <StackTrace.h>

SkeletalAnimation::SkeletalAnimation(const std::string& animationPath, Model* model)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(animationPath, aiProcess_Triangulate);
	assert(scene && scene->mRootNode);
	auto animation = scene->mAnimations[0];
	m_Duration = static_cast<float>(animation->mDuration);
	m_TicksPerSecond = static_cast<float>(animation->mTicksPerSecond);
	ReadHierarchyData(m_RootNode, scene->mRootNode);
	ReadMissingBones(animation, *model);


	m_CurrentTime = 0.0;

	m_FinalBoneMatrices.reserve(100);

	for (int i = 0; i < 100; i++)
		m_FinalBoneMatrices.push_back(glm::mat4(1.0f));
}

Bone* SkeletalAnimation::FindBone(const std::string& name)
{
	auto iter = std::find_if(m_Bones.begin(), m_Bones.end(),
		[&](const Bone& Bone)
		{
			return Bone.GetBoneName() == name;
		}
	);
	if (iter == m_Bones.end())
		return nullptr;
	else 
		return &(*iter);
}

void SkeletalAnimation::ReadMissingBones(const aiAnimation* animation, Model& model)
{
	int size = animation->mNumChannels;

	auto& boneInfoMap = model.GetBoneInfoMap();//getting m_BoneInfoMap from Model class
	int& boneCount = model.GetBoneCount(); //getting the m_BoneCounter from Model class

	//reading channels(bones engaged in an animation and their keyframes)
	for (int i = 0; i < size; i++)
	{
		auto channel = animation->mChannels[i];
		std::string boneName = channel->mNodeName.data;

		if (boneInfoMap.find(boneName) == boneInfoMap.end())
		{
			boneInfoMap[boneName].id = boneCount;
			boneCount++;
		}

		m_Bones.emplace_back(Bone(channel->mNodeName.data,
			boneInfoMap[channel->mNodeName.data].id, channel));
	}

	m_BoneInfoMap = boneInfoMap;
}

void SkeletalAnimation::ReadHierarchyData(AssimpNodeData& dest, const aiNode* src)
{
	assert(src);

	dest.name = src->mName.data;
	dest.transformation = Model::ConvertMatrixToGLMFormat(src->mTransformation);
	dest.childrenCount = src->mNumChildren;

	for (unsigned int i = 0; i < src->mNumChildren; i++)
	{
		AssimpNodeData newData;
		ReadHierarchyData(newData, src->mChildren[i]);
		dest.children.push_back(newData);
	}
}


void SkeletalAnimation::UpdateAnimation(float dt)
{
	m_DeltaTime = dt;
	m_CurrentTime += GetTicksPerSecond() * dt;
	m_CurrentTime = fmod(m_CurrentTime, GetDuration());
	CalculateBoneTransform(&(GetRootNode()), glm::mat4(1.0f));
}

void SkeletalAnimation::PlayAnimation(SkeletalAnimation* pAnimation)
{
	m_CurrentTime = 0.0f;
}

void SkeletalAnimation::CalculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform)
{
	if (!node) {
		std::cout << "node is null\n";
		return;
	}
	std::string nodeName = node->name;
	glm::mat4 nodeTransform = node->transformation;

	Bone* Bone = FindBone(nodeName);

	if (Bone)
	{
		Bone->Update(m_CurrentTime);
		nodeTransform = Bone->GetLocalTransform();
	}

	glm::mat4 globalTransformation = parentTransform * nodeTransform;

	auto boneInfoMap = GetBoneIDMap();
	if (boneInfoMap.find(nodeName) != boneInfoMap.end())
	{
		int index = boneInfoMap[nodeName].id;
		glm::mat4 offset = boneInfoMap[nodeName].offset;
		if(index < m_FinalBoneMatrices.size())
			m_FinalBoneMatrices[index] = globalTransformation * offset;
	}

	for (int i = 0; i < node->childrenCount; i++)
		CalculateBoneTransform(&node->children[i], globalTransformation);
}


std::ostream& operator<<(std::ostream& os, SkeletalAnimation& a)
{
	for(auto& b : a.getBones())
	{
		os << b;
	}	

	return os;
}
