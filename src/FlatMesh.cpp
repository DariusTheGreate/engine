#include <FlatMesh.h>
#include <OpenglWrapper.h>

FlatMesh::FlatMesh()
{
    this->vertices.resize(verticesRaw.size()/7);
    
    //TODO(darius) UB and stuff
    for(int i = 0; i < verticesRaw.size(); i += 7)
    {
        vertices[i / 7] = {};
        vertices[i / 7].Position = glm::vec3(verticesRaw[i], verticesRaw[i+1], verticesRaw[i+2]);
        vertices[i / 7].Normal = glm::vec3(verticesRaw[i+3], verticesRaw[i+4], 0);
        vertices[i / 7].TexCoords = glm::vec2(verticesRaw[i+5], verticesRaw[i+6]);
    }

    this->indices = indicesRaw;
    this->textures = {};

    this -> setupMesh();

    type = MeshType::FLAT;
} 

void FlatMesh::setTextureNormal(const std::string& path)
{
    //TODO(darius) make it single texture
    Texture texture(TextureFromFile(path.c_str(), false, false), path , "texture_normal");
    textures.push_back(texture);
}


void FlatMesh::setTextureParallax(const std::string& path)
{
   //TODO(darius) make it single texture
    Texture texture(TextureFromFile(path.c_str(), false, false), path , "texture_height");
    textures.push_back(texture); 
}

void FlatMesh::setTextureCoords(float x1 = 1.0, float y1 = 1.0, float x2 = 0.0, float y2 = 0.0)
{
    //std::unique_lock<std::mutex>(draw_mutex);
    //works
    vertices[0].TexCoords = glm::vec2{x1, y1};
    vertices[1].TexCoords = glm::vec2{x1, y2};
    vertices[2].TexCoords = glm::vec2{x2, y2};
    vertices[3].TexCoords = glm::vec2{x2, y1};
    this -> setupMesh();
}

void FlatMesh::DrawRaw(Shader& shader, glm::vec3 pos = { 0,0,0 }, glm::vec3 scale = {1,1,1})
{
    //std::unique_lock<std::mutex>(draw_mutex);
    glm::mat4 model = glm::mat4(1.0f);

    model = glm::translate(model, pos);
    model = glm::scale(model, scale);

    //shader.setMat4("model", model);

    vao.bind();
    OpenglWrapper::DrawElements(static_cast<int>(indices.size()));
    OpenglWrapper::UnbindVAO();
}

