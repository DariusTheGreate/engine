#pragma once

#include <glm/glm.hpp>
#include <vector>

class Mesh;
class Object;

class InstancedRenderer
{
public:
	void init();

	void setupMesh(Mesh* meshptr);

    void generateBuffer();

    void setupBuffer();

    void pushMat(glm::mat4 mat);

    std::vector<glm::mat4>& getMats()
    {
    	return instanceMats;
    }

    size_t size()
    {
    	return instanceMats.size();
    }

private:
    std::vector<glm::mat4> instanceMats;
    Mesh* mesh = nullptr;
    unsigned int buffer = 0;
};
