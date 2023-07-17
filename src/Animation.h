#pragma once

#include <vector>
#include <map>
#include <glm/glm.hpp>
#include <assimp/scene.h>
#include <Bone.h>
#include <functional>
#include <Model.h>
#include <FlatMesh.h>

#include <chrono>
#include <thread>

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

class SpriteAnimation
{
public:
	SpriteAnimation() = default;
    SpriteAnimation(std::vector<glm::vec4> points_in, float delay) : points(points_in), delayMs(delay) {}

    SpriteAnimation(float rows_in, float cols_in, float delay) :  delayMs(delay), rows(rows_in), cols(cols_in), length(rows*cols) 
	{
        initPoints();
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
		if ((currentTime - lastTime)*1000 < delayMs || !play)
			return;

		if (sprite) {
			if (points.size() > frameNum && frameNum < length) {
				int id = static_cast<int>(frameNum);
				sprite->setTextureCoords(points[id].x, points[id].y, points[id].z, points[id].w);
			}

			frameNum++;
			if (frameNum > points.size()-1 || frameNum > length)
				frameNum = start;

		}

		lastTime = currentTime;
	}

	void stop() 
	{
		//animationThread.join();
	}

    float* getDelay()
    {
        return &delayMs;
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

    FlatMesh* sprite = nullptr;

	//std::thread animationThread;
};

