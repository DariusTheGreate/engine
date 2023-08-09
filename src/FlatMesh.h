#pragma once

#include <Mesh.h>
#include <Model.h>
#include <texture.h>

#include <string>

class FlatMesh : public Mesh
{
public:
    FlatMesh();

    //TODO(darius) throws dangerous exception when name = Spid.png
    void setTexture(std::string path, std::string name);
    void setTexture(std::string path);



    void DrawRaw(Shader& shader, glm::vec3 pos, glm::vec3 scale);

    void setTextureCoords(float x1, float y1, float x2, float y2);
    
private:
    //TODO(darius) get rid of it?
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

