#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <Shader.h>
#include <Texture.h>
#include <VAO.h>
#include <VBO.h>
#include <EBO.h>
#include <FrustumCulling.h>
#include <Transform.h>

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

struct MeshAABB
{
    glm::vec3 min;
    glm::vec3 max;
    glm::vec3 center = {0,0,0};
    glm::vec3 size = {0,0,0}; 

    MeshAABB() = default;

    MeshAABB(const glm::vec3& min, const glm::vec3& max)
        : center{ (max + min) * 0.5f }, size{ max.x - center.x, max.y - center.y, max.z - center.z }
    {}

    MeshAABB(const glm::vec3& inCenter, float iI, float iJ, float iK)
        : center{ inCenter }, size{ iI, iJ, iK }
    {}

    bool isOnOrForwardPlane(const Plane& plane) 
    {
        float r = size.x * std::abs(plane.normal.x) + size.y * std::abs(plane.normal.y) +
            size.z * std::abs(plane.normal.z);

        return -r <= plane.getSignedDistanceToPlane(center);
    }

    bool isOnFrustum(Frustum& camFrustum, Transform& transform) 
    {
        //NOTE(darius) TEMPO while scale dont work
        if(transform.getScale() != glm::vec3{1,1,1})
            return true;

        glm::vec3 globalCenter{ transform.matrix * glm::vec4(center, 1.f) };

        glm::vec3 right = transform.getRight() * size.x;
        glm::vec3 up = transform.getUp() * size.y;
        glm::vec3 forward = transform.getForward() * size.z;

        float newIi = std::abs(glm::dot(glm::vec3{ 1.f, 0.f, 0.f }, right)) +
            std::abs(glm::dot(glm::vec3{ 1.f, 0.f, 0.f }, up)) +
            std::abs(glm::dot(glm::vec3{ 1.f, 0.f, 0.f }, forward));

        float newIj = std::abs(glm::dot(glm::vec3{ 0.f, 1.f, 0.f }, right)) +
            std::abs(glm::dot(glm::vec3{ 0.f, 1.f, 0.f }, up)) +
            std::abs(glm::dot(glm::vec3{ 0.f, 1.f, 0.f }, forward));

        float newIk = std::abs(glm::dot(glm::vec3{ 0.f, 0.f, 1.f }, right)) +
            std::abs(glm::dot(glm::vec3{ 0.f, 0.f, 1.f }, up)) +
            std::abs(glm::dot(glm::vec3{ 0.f, 0.f, 1.f }, forward));

        MeshAABB globalAABB(globalCenter, newIi, newIj, newIk);

        return (globalAABB.isOnOrForwardPlane(camFrustum.leftFace) &&
            globalAABB.isOnOrForwardPlane(camFrustum.rightFace) &&
            globalAABB.isOnOrForwardPlane(camFrustum.topFace) &&
            globalAABB.isOnOrForwardPlane(camFrustum.bottomFace) &&
            globalAABB.isOnOrForwardPlane(camFrustum.nearFace) &&
            globalAABB.isOnOrForwardPlane(camFrustum.farFace));
    };
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
    Mesh() noexcept = default;
  
    //TODO(darius) make it moveable for fuck sake!!!!
    //Mesh(cosnt Mesh& m) = delete;
    //Mesh(Mesh&& m) = default;
    
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

    void clearMesh() 
    {
        vao.deleteVAO();
        vbo.deleteVBO();
        ebo.deleteEBO();
    }

    void calculateAabb(Transform&);

    std::vector<unsigned int> generateLOD();

    //TODO(darius) speed up by using another thread for all the culling, and by not calculation aabb and global aabb each iteration
    bool cull(Frustum& camFrustum, Transform tr)
    {
        if(aabb.size == glm::vec3{0,0,0})
            calculateAabb(tr);
        return aabb.isOnFrustum(camFrustum, tr);
    }
    
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

