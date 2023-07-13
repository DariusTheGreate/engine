#include "Mesh.h"
#include <OpenglWrapper.h>

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures)
{
    this->vertices = vertices;
    this->indices = indices;
    this->textures = textures;

    setupMesh();
}

void Mesh::Draw(Shader& shader)
{
    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;
    unsigned int normalNr = 1;
    unsigned int heightNr = 1;

    for (unsigned int i = 0; i < textures.size(); i++)
    {
        glActiveTexture(GL_TEXTURE0 + i); 
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
    
    //TODO(darius) perfomance issues?
    //if(mode == DrawMode::DRAW_AS_ARRAYS)
	//	glDrawArrays(GL_TRIANGLES, 0, vertices.size());
    //else if(mode == DrawMode::DRAW_AS_INSTANCE)
    //    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, 1);
    //else            
    OpenglWrapper::DrawElements(static_cast<int>(indices.size()));

    OpenglWrapper::UnbindVAO();
    OpenglWrapper::ActivateTexture();
}

void Mesh::setupMesh()
{
    vao.init();
    vbo.init();
    ebo.init();

	vao.bind();
    vbo.bind(vertices.size() * sizeof(Vertex), &vertices[0]);
    
    ebo.bind(indices.size() * sizeof(unsigned int), &indices[0]);

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

