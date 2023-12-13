#pragma once

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
#include <Color.h>

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

class Object;

struct BoneInfo
{
    int id;
    glm::mat4 offset;
};

class Model
{
public:
    static std::unordered_set<std::string_view> textures_set;
    static std::vector<Texture> textures_loaded;//TODO(darius) create single buffer for all Textures data?
    
	std::vector<Mesh> meshes;//TODO(darius) create single buffer for all Mesh data?

    std::string directory;
    std::string path;

    std::map<std::string, BoneInfo> modelBoneInfoMap;
    int boneCounter = 0;

public:
    Model() = default;

    Model(std::string_view path_in, Shader& shader_in); 

    Model(const Mesh& mesh_in); 

    Model(const Model& m);

    Model(std::string_view path_in);

    void Draw(Object* obj, std::optional<PointLight>& light, std::optional<Material>& m);

    std::vector<Mesh> loadModel();

    void addMesh(const Mesh& m);

    auto& GetBoneInfoMap() { return modelBoneInfoMap; }

    int& GetBoneCount() { return boneCounter; } 

    void SetVertexBoneDataToDefault(Vertex& vertex);

    void SetVertexBoneData(Vertex& vertex, int boneID, float weight);

private:
    void processNode(aiNode* root, const aiScene* scene);

    Mesh processMesh(aiMesh* mesh, const aiScene* scene);

    std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, const std::string& typeName);

    void ExtractBoneWeightForVertices(std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene);
  
public:
    static glm::mat4 ConvertMatrixToGLMFormat(const aiMatrix4x4& from);

    static glm::vec3 GetGLMVec(const aiVector3D& vec); 

    static glm::quat GetGLMQuat(const aiQuaternion& pOrientation);
};
