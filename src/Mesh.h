#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <Shader.h>
#include <Texture.h>
#include <VAO.h>
#include <VBO.h>
#include <EBO.h>

#include <string>
#include <vector>
#include <thread>
#include <mutex>

constexpr int MAX_BONE_INFLUENCE = 4;

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    glm::vec3 Tangent;
    glm::vec3 Bitangent;
    int m_BoneIDs[MAX_BONE_INFLUENCE];
    float m_Weights[MAX_BONE_INFLUENCE];
};

enum class DrawMode
{
    DRAW_AS_ARRAYS,
    DRAW_AS_ELEMENTS,
    DRAW_AS_INSTANCE,
};

class Mesh {
public:
    Mesh() noexcept {}
  
    //TODO(darius) make it moveable for fuck sake!!!!
    //Mesh(cosnt Mesh& m) = delete;
    //Mesh(Mesh&& m) = default;
    
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
    
    void Draw(Shader& shader);
    void Draw(Shader& shader, size_t);

    void setDrawMode(DrawMode mode_in);

    std::vector<Vertex> getVertices();
    std::vector<unsigned int> getIndices();
    std::vector<Texture> getTextures();

    VAO getVao();
protected:
    //TODO(darius) memoryManage that
    //TODO(darius) recaclculate vertext normals after rotation
    std::vector<Vertex>       vertices;//TODO(techmo) serialize
    std::vector<unsigned int> indices;//TODO(techmo) serialize
    std::vector<Texture>      textures;//TODO(techmo) serialize


    VAO vao;
    VBO vbo;
    EBO ebo;
    bool initialized = false;


    DrawMode mode = DrawMode::DRAW_AS_ELEMENTS;
    
    //in order to synchronnize changes of vertecies data
    //std::mutex draw_mutex;

    void setupMesh();
};
