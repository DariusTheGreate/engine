#pragma once

#include <glad/glad.h> 

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stb_image.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <Shader.h>
#include <Mesh.h>
#include <Texture.h>
#include <Transform.h>
#include <PointLight.h>
#include <LightingShaderRoutine.h>
#include <SkeletalAnimationShaderRoutine.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <queue>
#include <unordered_set>
#include <string_view>
#include <functional>
#include <optional>

unsigned int TextureFromFile(const char* path, const std::string& directory, bool gamma, bool rotate);

struct BoneInfo
{
    int id;
    glm::mat4 offset;
};

class Model
{
public:
    static std::unordered_set<std::string_view> textures_set;
    static std::vector<Texture> textures_loaded;
    std::vector<Mesh>    meshes;

    Shader shader;
    LightingShaderRoutine shaderRoutine;
    std::optional<SkeletalAnimationShaderRoutine> animationShaderRoutine;

    std::string directory;
    std::string path;

    bool gammaCorrection = false;
    bool rotate = false;
    bool constructSubobjects = false;

    std::map<std::string, BoneInfo> m_BoneInfoMap;
    int m_BoneCounter = 0;

public:
    Model() {}

    Model(std::string_view path_in, Shader& shader_in, LightingShaderRoutine& shaderRoutine_in, bool gamma = false, bool rotate_in = false, bool constructSubobjects_in = false); 

    Model(Mesh mesh_in, Shader shader_in, LightingShaderRoutine shaderRoutine_in); 

    Model(Shader shader_in, LightingShaderRoutine& shaderRoutine_in);

    Model(std::string path_in, LightingShaderRoutine& sr, Shader shader_in, bool rotate_in = false);

    Model(const Model& m);

    Model(std::string_view path_in);

    //TODO(darius) make it shaderRoutine(tr[i]) for each mesh
    void Draw(Transform tr, std::optional<PointLight>& light, std::optional<Material>& m);

    std::vector<Mesh> loadModel();

    void addMesh(const Mesh& m);

    LightingShaderRoutine& getShaderRoutine();

    void setAnimationShaderRoutine(SkeletalAnimationShaderRoutine r);

    void setShader(Shader sdr);

    auto& GetBoneInfoMap() { return m_BoneInfoMap; }
    int& GetBoneCount() { return m_BoneCounter; } 

    void SetVertexBoneDataToDefault(Vertex& vertex);

    void SetVertexBoneData(Vertex& vertex, int boneID, float weight);

private:
    //TODO(darius) Bench it for recursive BFS, i think recursive is faster
    //TODO(darius) Dont use std::queue
    void processNode(aiNode* root, const aiScene* scene);

    Mesh processMesh(aiMesh* mesh, const aiScene* scene);

    std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);

    void ExtractBoneWeightForVertices(std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene);
  
public:
    static glm::mat4 ConvertMatrixToGLMFormat(const aiMatrix4x4& from);

    static glm::vec3 GetGLMVec(const aiVector3D& vec); 

    static glm::quat GetGLMQuat(const aiQuaternion& pOrientation);
};
