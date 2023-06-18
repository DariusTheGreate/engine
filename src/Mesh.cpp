#include "Mesh.h"

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

        glUniform1i(glGetUniformLocation(shader.getShader(), (name + number).c_str()), i);
        glBindTexture(GL_TEXTURE_2D, textures[i].get_texture());
    }

    vao.bind();
    
    //TODO(darius) perfomance issues?
    //if(mode == DrawMode::DRAW_AS_ARRAYS)
	//	glDrawArrays(GL_TRIANGLES, 0, vertices.size());
    //else if(mode == DrawMode::DRAW_AS_INSTANCE)
    //    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, 1);
    //else            
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);


    glBindVertexArray(0);

    glActiveTexture(GL_TEXTURE0);
}

void Mesh::setupMesh()
{
    vao.init();
    vbo.init();
    ebo.init();

	vao.bind();
    vbo.bind(vertices.size() * sizeof(Vertex), &vertices[0]);
    
    ebo.bind(indices.size() * sizeof(unsigned int), &indices[0]);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));

    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));
    
    glEnableVertexAttribArray(5);
    glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, m_BoneIDs));

    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_Weights));
    
    glBindVertexArray(0);
}
