#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <Shader.h>
#include <Texture.h>
#include <VAO.h>
#include <VBO.h>
#include <EBO.h>
#include <Transform.h>
#include <AABB.h>

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

enum class MeshType
{
    FLAT,
    CUBE,
    MESH,
};

struct Transform;

class Mesh {
public:
    //NOTE(darius) maybe work on that
    Mesh() noexcept = default;
    Mesh(const Mesh& m) = default;
    Mesh& operator =(const Mesh&) = default;
    Mesh(Mesh&& m) = default;
    Mesh& operator =(Mesh&&) = default;
    
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
    
    void Draw(Shader& shader, int instancedAmount = -1);

    DrawMode getDrawMode();

    void setDrawMode(DrawMode mode_in);

    std::vector<Vertex> getVertices();

    std::vector<Vertex>& getVerticesRef();

    std::vector<unsigned int> getIndices();

    std::vector<unsigned int>& getIndicesRef();

    std::vector<Texture> getTextures();

    std::vector<Texture>& getTexturesRef();

    void setTexture(const std::string& path, const std::string& name);

    void setTexture(const std::string& path);

    void printVertices();

    void addVerticesBath(Mesh& m, glm::vec3 shift);

    //TODO(darius) bad api
    void clearBatch(size_t verticesNum, size_t indicesNum);

    VAO getVao();

    MeshType getType();

    void clearMesh();

    void calculateAabb(Transform&);

    MeshAABB getAABB();

    std::vector<unsigned int> generateLOD();

    void updateAABB(Transform tr);
    
protected:
    //TODO(darius) memoryManage that
    //TODO(darius) recaclculate vertext normals after rotation
    //TODO(darius) make those vertices that are outside viewport/depth NaN (in vertex shader prbbly)
    std::vector<Vertex>       vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture>      textures;

    VAO vao;
    VBO vbo;
    EBO ebo;
    bool initialized = false;

    MeshAABB aabb;

    //NOTE(darius) probably not the best way to do it
    MeshType type = MeshType::MESH;

    DrawMode mode = DrawMode::DRAW_AS_ELEMENTS;
    
    //in order to synchronnize changes of vertecies data
    //std::mutex draw_mutex;

    void setupMesh();

    void prepareTextures(Shader& shader);

};

