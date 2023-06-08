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

class Model
{
public:
    static std::unordered_set<std::string_view> textures_set;
    static std::vector<Texture> textures_loaded;
    std::vector<Mesh>    meshes;

    Shader shader;
    LightingShaderRoutine shaderRoutine;

    std::string directory;
    std::string path;

    bool gammaCorrection = false;
    bool rotate = false;
    bool constructSubobjects = false;

    Model(std::string_view path_in, Shader& shader_in, LightingShaderRoutine& shaderRoutine_in, bool gamma = false, bool rotate_in = false, bool constructSubobjects_in = false) 
        : shader(shader_in), shaderRoutine(shaderRoutine_in), gammaCorrection(gamma), rotate(rotate_in), path(path_in)
    {
		loadModel();
    }

    Model(Mesh mesh_in, Shader shader_in, LightingShaderRoutine shaderRoutine_in) : shader(shader_in), shaderRoutine(shaderRoutine_in)
    {
        meshes.push_back(mesh_in);
    }

    Model(Shader shader_in, LightingShaderRoutine& shaderRoutine_in) : shader(shader_in), shaderRoutine(shaderRoutine_in)
    {
    }

    Model(std::string path_in, LightingShaderRoutine& sr, Shader shader_in, bool rotate_in = false) : path(path_in), rotate(rotate_in), shaderRoutine(sr), shader(shader_in)
    {
    }

    Model(const Model& m) : meshes(m.meshes), shader(m.shader), shaderRoutine(m.shaderRoutine)
    {

    }

    //TODO(darius) make it shaderRoutine(tr[i]) for each mesh
    void Draw(Transform tr, std::optional<PointLight>& light, std::optional<Material>& m)
    {
        if(light){
            light->setShaderLight(shader);
        }
        if(m){
            m->setShaderMaterial(shader);
        }
        shaderRoutine(tr);

        for (unsigned int i = 0; i < meshes.size(); i++) {
            //if(i == 3)
			//	shaderRoutine(Transform{(tr.position + glm::vec3{3,3,3}), tr.scale});
            meshes[i].Draw(shader);
        }
    }

    std::vector<Mesh> loadModel()
    {
        if (path == "")
            return {};
        Assimp::Importer importer;
        std::cout << "Read started...\n";
        const aiScene* scene = importer.ReadFile(path.data(), aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
        std::cout << "Read ended!\n";

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) 
        {
            std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
            return {};
        }
       
        directory = path.substr(0, path.find_last_of('/'));

        processNode(scene->mRootNode, scene);

        return meshes;
    }

    void addMesh(const Mesh& m)
    {
        meshes.push_back(m);
    }

    LightingShaderRoutine& getShaderRoutine()
    {
        return shaderRoutine; 
    }

private:
    //TODO(darius) Bench it for recursive BFS, i think recursive is faster
    //TODO(darius) Dont use std::queue
    void processNode(aiNode* root, const aiScene* scene)
    {
        std::queue<aiNode*> path;

        path.push(root);

        while (!path.empty()) {
            auto* curr = path.front();
            path.pop();

            for (unsigned int i = 0; i < curr->mNumMeshes; ++i)
            {
                aiMesh* mesh = scene->mMeshes[curr->mMeshes[i]];
                meshes.push_back(processMesh(mesh, scene));
            }
 
            for (unsigned int i = 0; i < curr->mNumChildren; i++)
			{
                path.push(curr -> mChildren[i]);
			}
        }
    }

    Mesh processMesh(aiMesh* mesh, const aiScene* scene)
    {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        std::vector<Texture> textures;

        for (unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            Vertex vertex;
            glm::vec3 vector; 

            vector.x = mesh->mVertices[i].x;
            vector.y = mesh->mVertices[i].y;
            vector.z = mesh->mVertices[i].z;
            vertex.Position = vector;

            if (mesh->HasNormals())
            {
                vector.x = mesh->mNormals[i].x;
                vector.y = mesh->mNormals[i].y;
                vector.z = mesh->mNormals[i].z;
                vertex.Normal = vector;
            }

            if (mesh->mTextureCoords[0]) 
            {
                glm::vec2 vec;
                vec.x = mesh->mTextureCoords[0][i].x;
                vec.y = mesh->mTextureCoords[0][i].y;
                vertex.TexCoords = vec;
                // tangent
                vector.x = mesh->mTangents[i].x;
                vector.y = mesh->mTangents[i].y;
                vector.z = mesh->mTangents[i].z;
                vertex.Tangent = vector;
                // bitangent
                vector.x = mesh->mBitangents[i].x;
                vector.y = mesh->mBitangents[i].y;
                vector.z = mesh->mBitangents[i].z;
                vertex.Bitangent = vector;
            }
            else
                vertex.TexCoords = glm::vec2(0.0f, 0.0f);

            vertices.push_back(vertex);
        }

        for (unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);
        }

        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

        // 1. diffuse maps
        std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        // 2. specular maps
        std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
        // 3. normal maps
        std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
        textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
        // 4. height maps
        std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
        textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

        return Mesh(vertices, indices, textures);
    }

    std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName)
    {
        std::vector<Texture> textures;
        for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
        {
            aiString str;

            mat->GetTexture(type, i, &str);
            std::cout << "textures: " << str.C_Str() << "\n";


            auto fnd = Model::textures_set.find(str.C_Str());
            if (fnd != Model::textures_set.end()) 
            {
                for (size_t j = 0; j < Model::textures_loaded.size(); ++j) {
                    if (std::strcmp(Model::textures_loaded[j].get_path().data(), (*fnd).data()) == 0) {
                        textures.push_back(Model::textures_loaded[j]);
                    }
                }
            }
            else 
            {
				Texture texture(TextureFromFile(str.C_Str(), this->directory, gammaCorrection, rotate), str.C_Str(), typeName);
                textures.push_back(texture);
                Model::textures_loaded.push_back(texture);
                Model::textures_set.insert(str.C_Str());
            }
        }
        return textures;
    }
};

