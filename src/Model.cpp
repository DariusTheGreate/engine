#include "Model.h"
#include <Printer.h>
#include <Renderer.h>
#include <OpenglWrapper.h>
#include <FrustumCulling.h>
#include <OcclusionCulling.h>

#include <iostream>

#include <stb_image.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


Model::Model(std::string_view path_in, Shader& shader_in, bool gamma, bool rotate_in, bool constructSubobjects_in) 
    : gammaCorrection(gamma), rotate(rotate_in), path(path_in)
{
    loadModel();
}

Model::Model(Mesh mesh_in) 
{
    meshes.push_back(mesh_in);
}

//Model::Model(std::string path_in, bool rotate_in) : path(path_in), rotate(rotate_in)
//{
//}

Model::Model(const Model& m) : meshes(m.meshes), path(m.path)
{

}

Model::Model(std::string_view path_in) : path(path_in){
    loadModel();
}

void Model::Draw(Object* obj, std::optional<PointLight>& light, std::optional<Material>& m)
{
    if(light){
        light->setShaderLight(Renderer::shaderLibInstance->getCurrShader());
    }
   
    if(m){
        m->setShaderMaterial(Renderer::shaderLibInstance->getCurrShader());
    }

    for (unsigned int i = 0; i < meshes.size(); i++) {

        //TODO(darius) move to separate thread?

        if(GameState::cullEnabled)
            meshes[i].updateAABB(obj->getTransform());//TODO(darius) mega slow 

        //OcclusionCuller::cull(meshes[i].getAABB(), {});

        //if(!GameState::cullEnabled || FrustumCuller::cull(meshes[i].getAABB()))
        if(FrustumCuller::cull(meshes[i].getAABB()))
        {
            //TODO(darius) cringe make beter architecture
            if(Renderer::shaderLibInstance->stage == ShaderLibrary::STAGE::ALBEDO || Renderer::shaderLibInstance->stage == ShaderLibrary::STAGE::SKELETAL){
                if(obj->getAnimator())
                    Renderer::shaderLibInstance->stage = ShaderLibrary::STAGE::SKELETAL;
                else
                    Renderer::shaderLibInstance->stage = ShaderLibrary::STAGE::ALBEDO;
            }

            Renderer::shaderLibInstance->shaderRoutine(obj);
            meshes[i].Draw(Renderer::shaderLibInstance->getCurrShader());
        }
    }
}

std::vector<Mesh> Model::loadModel()
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
   
    auto iter = path.find_last_of('/');
    if(iter == std::string::npos)
        iter = path.find_last_of('\\');

    directory = path.substr(0, iter);

    processNode(scene->mRootNode, scene);

    return meshes;
}

void Model::addMesh(const Mesh& m)
{
    meshes.push_back(m);
}

void Model::SetVertexBoneDataToDefault(Vertex& vertex)
{
    for (int i = 0; i < MAX_BONE_INFLUENCE; i++)
    {
        vertex.m_BoneIDs[i] = -1;
        vertex.m_Weights[i] = 0.0f;
    }
}

void Model::SetVertexBoneData(Vertex& vertex, int boneID, float weight)
{
    for (int i = 0; i < MAX_BONE_INFLUENCE; ++i)
    {
        if (vertex.m_BoneIDs[i] < 0)
        {
            vertex.m_Weights[i] = weight;
            vertex.m_BoneIDs[i] = boneID;
            break;
        }
    }
}

//TODO(darius) Bench it for recursive BFS, i think recursive is faster
//TODO(darius) Dont use std::queue
//TODO(darius) Either somehow reserve meshes. Or make meshes move
void Model::processNode(aiNode* root, const aiScene* scene)
{
    std::queue<aiNode*> path;

    path.push(root);
    meshes.reserve(scene -> mNumMeshes);

    while (!path.empty()) {
        auto* curr = path.front();
        path.pop();

        for (unsigned int i = 0; i < curr->mNumMeshes; ++i)
        {
            aiMesh* mesh = scene->mMeshes[curr->mMeshes[i]];
            meshes.emplace_back(processMesh(mesh, scene));
        }

        for (unsigned int i = 0; i < curr->mNumChildren; i++)
        {
            path.push(curr -> mChildren[i]);
        }
    }
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;

        SetVertexBoneDataToDefault(vertex);

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

    std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
    println("diffMaps size ", diffuseMaps.size());
    textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

    std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
    println("specMaps size ", specularMaps.size());
    textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

    std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
    println("normMaps size ", normalMaps.size());
    textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

    std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
    println("heightMaps size ", heightMaps.size());
    textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

    if (diffuseMaps.size() == 0) 
    {
        std::cout << "\nWARNING! ASSIMP COULDNT DISTINGHUISH MATERIALS. TRAVERSING THROUGH ALL MATERIALS\n";
        for (unsigned int m = 0; m < scene->mNumMaterials; ++m) 
        {
            aiMaterial* materialRepeat = scene->mMaterials[m];
            std::vector<Texture> diffuseMapsRepeat = loadMaterialTextures(materialRepeat, aiTextureType_DIFFUSE, "texture_diffuse");
            if (diffuseMapsRepeat.size() > 0) {
                textures.insert(textures.end(), diffuseMapsRepeat.begin(), diffuseMapsRepeat.end());
                break;
            }

            std::vector<Texture> normMapsRepeat = loadMaterialTextures(materialRepeat, aiTextureType_HEIGHT, "texture_normal");
            if (normMapsRepeat.size() > 0) {
                textures.insert(textures.end(), normMapsRepeat.begin(), normMapsRepeat.end());
                break;
            }
        }
    }
    
    //TODO(darius) check if textures that we ahve dont exist in filesystem
    if(textures.size() == 0)
    {
        print("\nWARNING! NO TEXTURES, LOADING DEFAULT TEXTURE\n");
        //Texture t(GameState::engine_path + "textures/checkerboard.png");
        //t.set_type("texture_diffuse"); 
        //textures.emplace_back(std::move(t)); 

        auto textID = TextureFromFile("checkerboard.png", GameState::engine_path + "textures", false, false); 

        Texture texture(textID, "checkerboard.png", "texture_diffuse");
        textures.push_back(texture); 
        println("TEXURES SIZE: ", textures.size());
    }

    ExtractBoneWeightForVertices(vertices,mesh,scene);

    return Mesh(vertices, indices, textures);
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, const std::string& typeName)
{
    std::vector<Texture> textures;

    if( mat->GetTextureCount(type) > 0)
    {
        aiString str;

        mat->GetTexture(type, 0, &str);

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
            auto textID = TextureFromFile(str.C_Str(), this->directory, gammaCorrection, rotate); 
            if(textID == 0)
                return textures;

            Texture texture(textID, str.C_Str(), typeName);
            textures.push_back(texture);
            Model::textures_loaded.push_back(texture);
            Model::textures_set.insert(str.C_Str());
        }
    }

    println("textures loaded size ", textures.size());

    return textures;
}

void Model::ExtractBoneWeightForVertices(std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene)
{
    println("Extracting Bones mNumBones: ", mesh->mNumBones);
    for (unsigned int boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex)
    {
        int boneID = -1;
        std::string boneName = mesh->mBones[boneIndex]->mName.C_Str();
        //println("Bone number ", boneIndex, ":", boneName);
        if (m_BoneInfoMap.find(boneName) == m_BoneInfoMap.end())
        {
            BoneInfo newBoneInfo;
            newBoneInfo.id = m_BoneCounter;
            newBoneInfo.offset = ConvertMatrixToGLMFormat(
                mesh->mBones[boneIndex]->mOffsetMatrix);
            //println("Bone Matrice: ", newBoneInfo.offset);
            m_BoneInfoMap[boneName] = newBoneInfo;
            boneID = m_BoneCounter;
            m_BoneCounter++;
        }
        else
        {
            boneID = m_BoneInfoMap[boneName].id;
        }
        assert(boneID != -1);
        auto weights = mesh->mBones[boneIndex]->mWeights;
        int numWeights = mesh->mBones[boneIndex]->mNumWeights;

        for (int weightIndex = 0; weightIndex < numWeights; ++weightIndex)
        {
            int vertexId = weights[weightIndex].mVertexId;
            float weight = weights[weightIndex].mWeight;
            assert(vertexId <= vertices.size());
            SetVertexBoneData(vertices[vertexId], boneID, weight);
        }
    }
}

glm::mat4 Model::ConvertMatrixToGLMFormat(const aiMatrix4x4& from)
{
    glm::mat4 to;
    //the a,b,c,d in assimp is the row ; the 1,2,3,4 is the column
    to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
    to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
    to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
    to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;
    return to;
}

glm::vec3 Model::GetGLMVec(const aiVector3D& vec) 
{ 
    return glm::vec3(vec.x, vec.y, vec.z); 
}

glm::quat Model::GetGLMQuat(const aiQuaternion& pOrientation)
{
    return glm::quat(pOrientation.w, pOrientation.x, pOrientation.y, pOrientation.z);
}

unsigned int TextureFromFile(const char* filename, bool gamma, bool rotate) 
{
    unsigned int textureID = 0;
    OpenglWrapper::GenerateTextures(&textureID);

    stbi_set_flip_vertically_on_load(rotate);
    int width, height, nrComponents;
    unsigned char* data = stbi_load(filename, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format = GL_RGBA;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        
        OpenglWrapper::BindTexture(static_cast<int>(textureID));
        OpenglWrapper::ImageTexture(format, width, height, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        OpenglWrapper::TextureParameter(GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
        OpenglWrapper::TextureParameter(GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
        OpenglWrapper::TextureParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        OpenglWrapper::TextureParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << filename << std::endl;
        stbi_image_free(data);
    }

    return static_cast<unsigned int>(textureID);

}

unsigned int TextureFromFile(const char* path, const std::string& directory, bool gamma, bool rotate = false)
{
    std::string filename = std::string(path);
    filename = directory + '/' + filename;
    return TextureFromFile(filename.c_str(), gamma, rotate);
}

std::unordered_set<std::string_view> Model::textures_set = {};
std::vector<Texture> Model::textures_loaded = {};

