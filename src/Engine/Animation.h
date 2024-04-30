#pragma once

#include <vector>
#include <map>
#include <glm/glm.hpp>
#include <assimp/scene.h>
#include <Engine/Bone.h>

#include <Rendering/FlatMesh.h>
#include <Rendering/Model.h>

#include <functional>

#include <chrono>
#include <thread>
#include <filesystem>
#include <variant>

class Model;

struct AssimpNodeData
{
	glm::mat4 transformation;
	std::string name;
	int childrenCount;
	std::vector<AssimpNodeData> children;
};

//IMPORTANT(darius) Animation system needs some hierarchy. 
//					Now theres nothing in common between ANimation and SpriteAnimation. 
//					Which is, i think, actually, good. But we need to work on this. 

// ALSO MAKE UR OWN FKN ANIMATION FORMAT AND CONVERTORS, CAUSE THIS FCKN FBX DAE HUE FILES ARE SO FKIN BS THERE IS FKN SIMDJSON THAT FAST AS FUCK SO USE IT INSTEAD
// SEE: https://www.youtube.com/watch?v=sQKAoiMPPOQ


struct AnimationBase
{
	float length = 0;
	float currTime = 0;
	//TODO(darius) make same interface for update dt not the same as currTime and its soo cringe

	bool isOver()
	{
		return currTime == length;
	}

	void reset()
	{
		currTime = 0;
	}
};

class SkeletalAnimation : public AnimationBase
{
public:
	SkeletalAnimation(const std::string& animationPath, Model* model);

	SkeletalAnimation(const SkeletalAnimation& anim) = default;

	Bone* findBone(const std::string& name);

	void update(float dt);

	void calculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform);

	std::vector<Bone>& getBones();

	float getLength(); 

	const std::map<std::string,BoneInfo>& getBoneIDMap(); 

	std::vector<glm::mat4> getFinalBoneMatrices();

private:
	void readMissingBones(const aiAnimation* animation, Model& model);

	void readHierarchyData(AssimpNodeData& dest, const aiNode* src);

private:
	float deltaTime;
	std::vector<glm::mat4> finalBoneMatrices;//NOTE(darius) maybe use Transform here?

	AssimpNodeData rootNode;
	std::vector<Bone> bones;
	std::map<std::string, BoneInfo> boneInfoMap;
};

std::ostream& operator<<(std::ostream& os, SkeletalAnimation& a);

class SpriteAnimation : public AnimationBase
{
public:
	SpriteAnimation() = default;
    SpriteAnimation(std::vector<glm::vec4> points_in, float delay) : points(points_in), delayMs(delay) {}

    SpriteAnimation(float rows_in, float cols_in, float delay);

	SpriteAnimation(std::string animationFolderPathStr);

    void initPoints();

	void setSprite(FlatMesh* mesh);

	void update(float currentTime);

	void stop(); 

    float* getDelay();

    void setDelay(float delay);

    float* getRows();

    float* getCols();

    float* getLength();

	float* getStart(); 
	
	float* getBorder(); 

	bool* getPlay(); 

	std::vector<glm::vec4> getPoints(); 

	bool isAnimationUsesMultipleTextures();

	std::string_view getAnimationFolderPath();

private:
    std::vector<glm::vec4> points;
	float lastTime = 0;
	float delayMs = 500;

    float rows = 0;
    float cols = 0;

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

template<typename AnimationT>
class Animator
{
public:
	void addAnimation(const AnimationT& anim)
	{
		animations.push_back(anim);

		for(int i = 0; i < animationsGraph.size(); ++i){
			animationsGraph[i].resize(animations.size());
		}

		animationsGraph.emplace_back(std::vector<size_t>(animations.size()));
	}

	//NOTE(darius) beware that you could updating animations but not render or rendering the wrong ones
	void update(float t)
	{
		if(!animations.size())
			return;

		//TODO(darius) work here
		if(!animations[currAnim].isOver()){
			animations[currAnim].update(t);
		}
		else{
			animations[currAnim].reset();
			currAnim = getNextAnimFromGraphLink(currAnim);
			//Timer::printCurrentTime();
		}
	}

	AnimationT& getCurrentAnimation()
	{
		return animations[currAnim];	
	}

	std::vector<AnimationT>& getAnimations()
	{
		return animations;
	}

	void setAnimationsGraph(std::vector<std::vector<size_t>>&& graph_in)
	{
		animationsGraph = std::move(graph_in);
	}

	std::vector<std::vector<size_t>>& getAnimationsGraph()
	{
		return animationsGraph;
	}

	size_t size()
	{
		return animations.size();
	}

	void nextAnim()
	{
		currAnim++;
		if(currAnim > size()-1)
			currAnim = 0;
	}

	size_t currPlayedAnim()
	{
		return currAnim;
	}

	void setCurrAnim(int currAnimId)
	{
		if(currAnimId < 0)
			currAnimId = 0;

		currAnim = currAnimId;
	}

	void addLinkToGraph(size_t animA, size_t animB)
	{
		assert(animA < animations.size() && animB < animations.size());
		animationsGraph[animA][animB] = 1;
	}

	size_t getNextAnimFromGraphLink(size_t animA)
	{
		if(animA >= animationsGraph.size())
			return animA;

		for(size_t i = 0; i < animationsGraph[animA].size(); ++i){
			if(animationsGraph[animA][i])
				return i;
		}

		return animA;
	}

private:
	std::vector<AnimationT> animations;
	//NOTE(darius) maybe use vector<int> graph[i] = j => i -> j
	std::vector<std::vector<size_t>> animationsGraph;//TODO(darius) optimise it, but ok for now
	size_t currAnim = 0;
};

using SpriteAnimator = Animator<SpriteAnimation>;
