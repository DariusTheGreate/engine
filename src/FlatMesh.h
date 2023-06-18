#pragma once

#include <Mesh.h>
#include <Model.h>
#include <texture.h>

#include <string>

class FlatMesh : public Mesh
{
public:
    FlatMesh();

    void setTexture(std::string path, std::string name);

    void DrawRaw(Shader& shader, size_t sdrp);

    //TODO(darius) instanced rendering here
    void Draw(Shader& shader, size_t count);
    
private:
    std::vector<float> verticesRaw = {
         0.5f,  0.5f, 0.0f,  0.0f, 0.0f,  1.0f, 1.0f,
         0.5f, -0.5f, 0.0f,  1.0f, 0.0f,  1.0f, 0.0f,
        -0.5f, -0.5f, 0.0f,  1.0f, 1.0f,  0.0f, 0.0f,
        -0.5f,  0.5f, 0.0f,  0.0f, 1.0f,  0.0f, 1.0f,  
    };

    std::vector<unsigned int> indicesRaw = {
        0, 1, 3,
        1, 2, 3
    };
    
}; 
