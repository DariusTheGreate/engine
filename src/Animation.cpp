#include <Animation.h>
#include <StackTrace.h>

SkeletalAnimation::SkeletalAnimation(const std::string& animationPath, Model* model)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(animationPath, aiProcess_Triangulate);
	assert(scene && scene->mRootNode);
	auto animation = scene->mAnimations[0];
	length = static_cast<float>(animation->mDuration);
	deltaTime = static_cast<float>(animation->mTicksPerSecond);
	readHierarchyData(rootNode, scene->mRootNode);
	readMissingBones(animation, *model);

	currTime = 0.0;

	//note: reserve(100)
	finalBoneMatrices.reserve(length);

	for (int i = 0; i < length; i++)
		finalBoneMatrices.push_back(glm::mat4(1.0f));
}

Bone* SkeletalAnimation::findBone(const std::string& name)
{
	for(auto iter = bones.begin(); iter != bones.end(); iter++)
	{
		if(iter->GetBoneName() == name)
			return &(*iter);
	}
	
	return nullptr;	
}

void SkeletalAnimation::readMissingBones(const aiAnimation* animation, Model& model)
{
	int size = animation->mNumChannels;

	auto& modelBoneInfoMap = model.GetBoneInfoMap();
	int& boneCount = model.GetBoneCount(); 

	for (int i = 0; i < size; i++)
	{
		auto channel = animation->mChannels[i];
		std::string boneName = channel->mNodeName.data;

		if (modelBoneInfoMap.find(boneName) == modelBoneInfoMap.end())
		{
			modelBoneInfoMap[boneName].id = boneCount;
			boneCount++;
		}

		bones.emplace_back(Bone(channel->mNodeName.data,
			modelBoneInfoMap[channel->mNodeName.data].id, channel));
	}

	boneInfoMap = modelBoneInfoMap;
}

void SkeletalAnimation::readHierarchyData(AssimpNodeData& dest, const aiNode* src)
{
	assert(src);

	dest.name = src->mName.data;
	dest.transformation = Model::ConvertMatrixToGLMFormat(src->mTransformation);
	dest.childrenCount = src->mNumChildren;

	for (unsigned int i = 0; i < src->mNumChildren; i++)
	{
		AssimpNodeData newData;
		readHierarchyData(newData, src->mChildren[i]);
		dest.children.push_back(newData);
	}
}

void SkeletalAnimation::update(float dt)
{
	currTime += deltaTime * dt;
	currTime = fmod(currTime, getLength());
	println("update skeletal ", dt, " ", currTime);
	calculateBoneTransform(&(rootNode), glm::mat4(1.0f));
}

void SkeletalAnimation::calculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform)
{
	if (!node) {
		return;
	}
	std::string nodeName = node->name;
	glm::mat4 nodeTransform = node->transformation;

	Bone* Bone = findBone(nodeName);

	if (Bone)
	{
		Bone->Update(currTime);
		nodeTransform = Bone->GetLocalTransform();
	}

	glm::mat4 globalTransformation = parentTransform * nodeTransform;

	if (boneInfoMap.find(nodeName) != boneInfoMap.end())
	{
		int index = boneInfoMap[nodeName].id;
		glm::mat4 offset = boneInfoMap[nodeName].offset;
		if(index < finalBoneMatrices.size())
			finalBoneMatrices[index] = globalTransformation * offset;
	}

	for (int i = 0; i < node->childrenCount; i++)
		calculateBoneTransform(&node->children[i], globalTransformation);
}


std::vector<Bone>& SkeletalAnimation::getBones()
{
	return bones;
}

float SkeletalAnimation::getLength() 
{ 
	return length;
}

const std::map<std::string,BoneInfo>& SkeletalAnimation::getBoneIDMap() 
{ 
	return boneInfoMap;
}

std::vector<glm::mat4> SkeletalAnimation::getFinalBoneMatrices()
{
	return finalBoneMatrices;
}

std::ostream& operator<<(std::ostream& os, SkeletalAnimation& a)
{
	for(auto& b : a.getBones())
	{
		os << b;
	}	

	return os;
}


SpriteAnimation::SpriteAnimation(float rows_in, float cols_in, float delay) :  delayMs(delay), rows(rows_in), cols(cols_in)
{
	length = rows*cols;
    initPoints();
}

SpriteAnimation::SpriteAnimation(std::string animationFolderPathStr) : pathToFolder(animationFolderPathStr)
{
	animationUseMultipleTextures = true;//!

	namespace fs = std::filesystem;

	using directory_iterator = fs::directory_iterator;
	using f_path = fs::path;

	//NOTE(darius) important to .c_str()
	f_path animationFolderPath{ animationFolderPathStr.c_str()};

	for (const auto& dirEntry : directory_iterator(animationFolderPath)) {
		if (f_path{ dirEntry }.extension() == ".png") {
			std::cout << dirEntry.path().string() << std::endl;
			FlatMesh m;
			m.setTexture((dirEntry.path().string()));
			spritesList.push_back(m);
		}
	}

	length = static_cast<float>(spritesList.size());
}

void SpriteAnimation::initPoints()
{
    points.clear();
	float dc = border * (length - 1) * 0.001f;
    for (float i = 1; i <= rows; ++i) {
		for (float j = 1; j <= cols; ++j) 
		{
			points.push_back(glm::vec4((j-1)/cols + dc,(i-1)/rows, j/cols - dc, i/rows));
		}
	}
}

void SpriteAnimation::setSprite(FlatMesh* mesh)
{
	sprite = mesh;
}

void SpriteAnimation::update(float currentTime) 
{
	//TODO(darius) refactor this
	if ((currentTime - lastTime)*1000 < delayMs || !play)
		return;

	if (sprite && !animationUseMultipleTextures) {
		if (points.size() > currTime && currTime < length) {
			int id = static_cast<int>(currTime);
			sprite->setTextureCoords(points[id].x, points[id].y, points[id].z, points[id].w);
		}

		//currTime++;
		//if (currTime > points.size()-1 || currTime > length)
		//	currTime = start;

		currTime = fmod(++currTime, points.size()-1);
	}
	
	if (animationUseMultipleTextures) 
	{
		if(spritesList.size() > currTime && currTime < length)
		{
			int id = static_cast<int>(currTime);
			//sprite = &spritesList[id];//NOTE(darius) invalidation?

			if (sprite->getTexturesRef().size() == 0)
				sprite->getTexturesRef().resize(1);

			sprite->getTexturesRef()[0] = spritesList[id].getTexturesRef()[0];
		}

		currTime = fmod(++currTime, spritesList.size());
	}

	lastTime = currentTime;
}

void SpriteAnimation::stop() 
{
	play = false;
}

float* SpriteAnimation::getDelay()
{
    return &delayMs;
}

void SpriteAnimation::setDelay(float delay)
{
	delayMs = delay;
}

float* SpriteAnimation::getRows()
{
    return &rows;
}

float* SpriteAnimation::getCols()
{
    return &cols;
}

float* SpriteAnimation::getLength()
{
    return &length;
}

float* SpriteAnimation::getStart() 
{
	return &start;
}

float* SpriteAnimation::getBorder() 
{
	return &border;
}

bool* SpriteAnimation::getPlay() 
{
	return &play;
}

std::vector<glm::vec4> SpriteAnimation::getPoints() 
{
	return points;
}

bool SpriteAnimation::isAnimationUsesMultipleTextures()
{
	return animationUseMultipleTextures;
}

std::string_view SpriteAnimation::getAnimationFolderPath()
{
	return pathToFolder;
}
