#pragma once
#include "stb_image.h"
#include <vector>
#include <string>

#include <Rendering/VAO.h>
#include <Rendering/VBO.h>
#include <Rendering/EBO.h>

#include <Rendering/Texture.h>

class Terrain
{
public:
	Terrain(std::string heightMap);

	void draw();

private:
	int numStrips = 0;
	int numTrisPerStrip = 0;

    //TODO(darius) create Mesh for it 
    VAO terrainVAO;
    VBO terrainVBO;
    EBO terrainEBO;

    Texture texture;

    std::vector<float> vertices;
    std::vector<unsigned> indices;
};
