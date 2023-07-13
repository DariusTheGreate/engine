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
} 

void FlatMesh::setTexture(std::string path, std::string name)
{
    //TODO(darius) make it single texture
    Texture texture(TextureFromFile(name.c_str(), path.c_str(), false, false), name, "texture_diffuse");
    textures.push_back(texture);
}

void FlatMesh::DrawRaw(Shader& shader, size_t sdrp, glm::vec3 pos = { 0,0,0 }, glm::vec3 scale = {1,1,1})
{
    glm::mat4 model = glm::mat4(1.0f);

    model = glm::translate(model, pos);
    model = glm::scale(model, scale);

    shader.setMat4("model", model);

    vao.bind();
    OpenglWrapper::DrawElements(static_cast<int>(indices.size()));
    OpenglWrapper::UnbindVAO();
}

//TODO(darius) instanced rendering here
void FlatMesh::Draw(Shader& shader, size_t amount)
{
    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;
    unsigned int normalNr = 1;
    unsigned int heightNr = 1;

    for (unsigned int i = 0; i < textures.size(); i++)
    {
        OpenglWrapper::ActivateTexture(GL_TEXTURE0 + i); 
        std::string number;
        std::string name = textures[i].get_type();
        if (name == "texture_diffuse")
            number = std::to_string(diffuseNr++);
        else if (name == "texture_specular")
            number = std::to_string(specularNr++); 
        else if (name == "texture_normal")
            number = std::to_string(normalNr++); 
        else if (name == "texture_height")
            number = std::to_string(heightNr++); 

        OpenglWrapper::SetShaderInt(shader.getShader(), (name + number).c_str(), i);
        OpenglWrapper::BindTexture(static_cast<int>(textures[i].get_texture()));
    }

    vao.bind();

    //OpenglWrapper::DrawInstances(36, amount);

    OpenglWrapper::DrawElements(static_cast<int>(indices.size()));
    //OpenglWrapper::DrawArrays(36);
    OpenglWrapper::UnbindVAO();
    OpenglWrapper::BindTexture(GL_TEXTURE0);
}

