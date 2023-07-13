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
	int m_TicksPerSecond;
	std::vector<Bone> m_Bones;
	AssimpNodeData m_RootNode;
	std::map<std::string, BoneInfo> m_BoneInfoMap;
};

class SpriteAnimation
{
public:
    SpriteAnimation(std::vector<glm::vec4> points_in, int delay) : points(points_in), delayMs(delay) {}
    SpriteAnimation(float rows_in, float cols_in, int delay) :  delayMs(delay), rows(rows_in), cols(cols_in), length(rows*cols) 
	{
        initPoints();
    }

    void initPoints()
    {
        points.clear();
        for (float i = 1; i <= rows; ++i) {
			for (float j = 1; j <= cols; ++j) 
			{
				points.push_back(glm::vec4((j-1)/cols,(i-1)/rows, j/cols, i/rows));
			}
		}
    }


	void setSprite(FlatMesh* mesh)
	{
		sprite = mesh;
	}

	//NOTE(darius) for future, make it thread pool and stuff. Same for ParticleSystem 
	void play() 
	{
		 animationThread = std::thread([this](int delay){
			while (1) {
				for (auto p : points)
				{
					std::this_thread::sleep_for(std::chrono::milliseconds(1000));
					std::cout << "pospal\n";
					if(sprite)
						sprite->setTextureCoords(p.x, p.y, 0.0, 0.0);
				}
			}
        }, delayMs);
	}

	void update(float currentTime) 
	{
		if ((currentTime - lastTime)*1000 < delayMs)
			return;

		if (sprite) {
            if(points.size() > frameNum && frameNum < length)
                sprite->setTextureCoords(points[frameNum].x, points[frameNum].y, points[frameNum].z, points[frameNum].w);

			frameNum++;
			if (frameNum > points.size()-1)
				frameNum = 0;

		}

		lastTime = currentTime;
	}

	void stop() 
	{
		animationThread.join();
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


private:
    std::vector<glm::vec4> points;

	int frameNum = 0;
	float lastTime = 0;
	float delayMs = 500;
    float rows = 0;
    float cols = 0;
    float length = 0;

    FlatMesh* sprite = nullptr;

	std::thread animationThread;
};

