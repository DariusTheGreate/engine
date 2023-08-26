#include "Mesh.h"
#include <OpenglWrapper.h>
#include <Renderer.h>

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures)
{
    this->vertices = vertices;
    this->indices = indices;
    this->textures = textures;

    setupMesh();
}

void Mesh::setDrawMode(DrawMode mode_in)
{
    mode = mode_in;
}

std::vector<Vertex> Mesh::getVertices()
{
    return vertices;
}

std::vector<unsigned int> Mesh::getIndices()
{
    return indices;
}

std::vector<Texture> Mesh::getTextures()
{
    return textures;
}

std::vector<Texture>& Mesh::getTexturesRef()
{
    return textures;
}

void Mesh::setTexture(std::string path, std::string name)
{
    //TODO(darius) make it single texture
    Texture texture(TextureFromFile(name.c_str(), path.c_str(), false, false), path + "/" + name, "texture_diffuse");
    textures.push_back(texture);
}

void Mesh::setTexture(std::string path)
{
    //TODO(darius) make it single texture
    Texture texture(TextureFromFile(path.c_str(), false, false), path , "texture_diffuse");
    textures.push_back(texture);
}

VAO Mesh::getVao()
{
    return vao;
}

DrawMode Mesh::getDrawMode()
{
    return mode;
}

void Mesh::Draw(Shader& shader)
{
    //std::cout << "positions " << vertices[0].Position.x << " " << vertices[0].Position.x << vertices[0].Position.y << "\n";
    //std::unique_lock<std::mutex>(draw_mutex);
    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;
    unsigned int normalNr = 1;
    unsigned int heightNr = 1;

    vao.bind();

    /*if (Renderer::shaderLibInstance->stage == ShaderLibrary::STAGE::SHADOWS)
    {
        std::cout << "suka\n";
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, static_cast<int>(textures[0].get_texture()));
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, Renderer::shaderLibInstance->depthMap);
    }

    else*/
    {
        for (unsigned int i = 0; i < textures.size(); i++)
        {
            glActiveTexture(GL_TEXTURE0 + i);
            std::string number;
            std::string name = textures[i].get_type();
            if (name == "texture_diffuse"){
                number = std::to_string(diffuseNr++);
            }
            else if (name == "texture_specular")
                number = std::to_string(specularNr++);
            else if (name == "texture_normal")
                number = std::to_string(normalNr++);
            else if (name == "texture_height")
                number = std::to_string(heightNr++);

            OpenglWrapper::SetShaderInt(shader.getShader(), (name + number).c_str(), i);
            OpenglWrapper::BindTexture(static_cast<int>(textures[i].get_texture()));

            //OpenglWrapper::BindTexture(Renderer::shaderLibInstance->depthMap);
            //OpenglWrapper::ActivateTexture();
        }

    }

    //TODO(darius) perfomance issues?
    if(mode == DrawMode::DRAW_AS_ARRAYS)
		glDrawArrays(GL_TRIANGLES, 0, vertices.size());
    else if(mode == DrawMode::DRAW_AS_INSTANCE)
        glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, 1);
    else            
        OpenglWrapper::DrawElements(static_cast<int>(indices.size()));

    //NOTE(darius) to draw cubeMesh
    //glDrawArrays(GL_TRIANGLES, 0, 36);
    //OpenglWrapper::UnbindVAO();
}

void Mesh::Draw(Shader& shader, size_t amount)
{
    // std::unique_lock<std::mutex>(draw_mutex);
    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;
    unsigned int normalNr = 1;
    unsigned int heightNr = 1;

    for (unsigned int i = 0; i < textures.size(); i++)
    {

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
        OpenglWrapper::ActivateTexture(GL_TEXTURE0 + i);
        OpenglWrapper::BindTexture(static_cast<int>(textures[i].get_texture()));
    }

    vao.bind();

    //OpenglWrapper::DrawInstances(36, amount);
    glDrawElementsInstanced(
        GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0, amount
    );
    //glDrawArraysInstanced(
    //    GL_TRIANGLES, 0, 36, amount
    //);

    //OpenglWrapper::DrawElements(static_cast<int>(indices.size()));
    //OpenglWrapper::DrawArrays(36);
    OpenglWrapper::UnbindVAO();
    //OpenglWrapper::BindTexture(GL_TEXTURE0);
}

void Mesh::setupMesh()
{
    //std::unique_lock<std::mutex>(draw_mutex);

    //DANGER(darius) TODO(darius) reinit, if was initialized before
    if (!initialized) {
        vao.init();
        vbo.init();
        ebo.init();
        initialized = true;
    }

	vao.bind();
    vbo.bind(vertices.size() * sizeof(Vertex), &vertices[0]);
    
    ebo.bind(indices.size() * sizeof(unsigned int), &indices[0]);

    //NOTE(darius) TODO(darius) try batching approach using glBufferSubData(...)?
    OpenglWrapper::EnableAttribute(0);
    OpenglWrapper::AttributePointer(0, 3, GL_FLOAT, sizeof(Vertex), (void*)0);

    OpenglWrapper::EnableAttribute(1);
    OpenglWrapper::AttributePointer(1, 3, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

    OpenglWrapper::EnableAttribute(2);
    OpenglWrapper::AttributePointer(2, 2, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

    OpenglWrapper::EnableAttribute(3);
    OpenglWrapper::AttributePointer(3, 3, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));

    OpenglWrapper::EnableAttribute(4);
    OpenglWrapper::AttributePointer(4, 3, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));
    
    OpenglWrapper::EnableAttribute(5);
    OpenglWrapper::AttributePointer(5, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, m_BoneIDs));

    OpenglWrapper::EnableAttribute(6);
    OpenglWrapper::AttributePointer(6, 4, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, m_Weights));
    
    OpenglWrapper::UnbindVAO();
}

