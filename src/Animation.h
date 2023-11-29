#pragma once

#include <vector>
#include <map>
#include <glm/glm.hpp>
#include <assimp/scene.h>
#include <Bone.h>
#include <functional>
#include <Model.h>
#include <FlatMesh.h>
#include <Printer.h>

#include <chrono>
#include <thread>
#include <filesystem>

struct AssimpNodeData
{
	glm::mat4 transformation;
	std::string name;
	int childrenCount;
	std::vector<AssimpNodeData> children;
};

//IMPORTANT(darius) Animation system needs some ierarchy. 
//					Now theres nothing in common between ANimation and SpriteAnimation. 
//					Which is, i think, actually, good. But we need to work on this. 

// ALSO MAKE UR OWN FKN ANIMATION FORMAT AND CONVERTORS, CAUSE THIS FCKN FBX DAE HUE FILES ARE SO FKIN BS THERE IS FKN SIMDJSON THAT FAST AS FUCK SO USE IT INSTEAD
class SkeletalAnimation
{
public:
	SkeletalAnimation(const std::string& animationPath, Model* model);

	SkeletalAnimation(const SkeletalAnimation& anim) = default;

	Bone* FindBone(const std::string& name);

	std::vector<Bone>& getBones()
	{
		return m_Bones;
	}
	
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

	void UpdateAnimation(float dt);

	void PlayAnimation(SkeletalAnimation* pAnimation);

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
	void ReadMissingBones(const aiAnimation* animation, Model& model);

	void ReadHierarchyData(AssimpNodeData& dest, const aiNode* src);

private:
	float m_Duration;
	float m_TicksPerSecond;
	std::vector<Bone> m_Bones;
	AssimpNodeData m_RootNode;
	std::map<std::string, BoneInfo> m_BoneInfoMap;


	std::vector<glm::mat4> m_FinalBoneMatrices;
	float m_CurrentTime = 0;
	float m_DeltaTime = 0;
};

std::ostream& operator<<(std::ostream& os, SkeletalAnimation& a);

class SpriteAnimation
{
public:
	SpriteAnimation() = default;
    SpriteAnimation(std::vector<glm::vec4> points_in, float delay) : points(points_in), delayMs(delay) {}

    SpriteAnimation(float rows_in, float cols_in, float delay) :  delayMs(delay), rows(rows_in), cols(cols_in), length(rows*cols) 
	{
        initPoints();
    }

	SpriteAnimation(std::string animationFolderPathStr) : pathToFolder(animationFolderPathStr)
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

    void initPoints()
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

	void setSprite(FlatMesh* mesh)
	{
		sprite = mesh;
	}

	void update(float currentTime) 
	{
		//TODO(darius) refactor this
		if ((currentTime - lastTime)*1000 < delayMs || !play)
			return;

		if (sprite && !animationUseMultipleTextures) {
			if (points.size() > frameNum && frameNum < length) {
				int id = static_cast<int>(frameNum);
				sprite->setTextureCoords(points[id].x, points[id].y, points[id].z, points[id].w);
			}

			frameNum++;
			if (frameNum > points.size()-1 || frameNum > length)
				frameNum = start;
		}
		
		if (animationUseMultipleTextures) 
		{
			if(spritesList.size() > frameNum && frameNum < length)
			{
				int id = static_cast<int>(frameNum);
				//sprite = &spritesList[id];//NOTE(darius) invalidation?

				if (sprite->getTexturesRef().size() == 0)
					sprite->getTexturesRef().resize(1);

				sprite->getTexturesRef()[0] = spritesList[id].getTexturesRef()[0];
			}

			frameNum++;
			if (frameNum > spritesList.size() - 1 || frameNum > length)
				frameNum = start;
		}

		lastTime = currentTime;
	}

	void stop() 
	{
		play = false;
	}

    float* getDelay()
    {
        return &delayMs;
    }

    void setDelay(float delay)
    {
    	delayMs = delay;
    }

    float* getRows()
    {
        return &rows;
    }

    float* getCols()
    {
        return &cols;
    }

    float* getLength()
    {
        return &length;
    }

	float* getStart() 
	{
		return &start;
	}
	
	float* getBorder() 
	{
		return &border;
	}

	bool* getPlay() 
	{
		return &play;
	}

	std::vector<glm::vec4> getPoints() 
	{
		return points;
	}

	bool isAnimationUsesMultipleTextures()
	{
		return animationUseMultipleTextures;
	}

	std::string_view getAnimationFolderPath()
	{
		return pathToFolder;
	}

private:
    std::vector<glm::vec4> points;

	float frameNum = 0;
	float lastTime = 0;
	float delayMs = 500;

    float rows = 0;
    float cols = 0;

    float length = 0;
	float start = 0;
	float border = 0;

	bool play = true;

	//TODO(darius) make it ID or something
    FlatMesh* sprite = nullptr;

	//NOTE(darius) this is used in case we wont spriteAnimation to use multiple separated texture files,
	//			   and not single texture Atlas. Bad think is that even if we use texture atlas we store this vector.. 
	//             But alternative is to do som hierarchy magic that i dont want to do now, plus i have a feeling that its faster this way.
	//TODO(darius) combine this multiple textures into single one (easy)
	std::vector<FlatMesh> spritesList;
	bool animationUseMultipleTextures = false;
	std::string pathToFolder;
};

