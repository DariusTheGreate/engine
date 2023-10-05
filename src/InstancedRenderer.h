#pragma once
#include <Mesh.h>

class InstancedRenderer
{
public:
	void init();

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
    Mesh mesh;

    unsigned int buffer = 0;
};
