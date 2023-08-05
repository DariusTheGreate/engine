#include "CubeMesh.h"

CubeMesh::CubeMesh()
{
    this->vertices.resize(verticesRaw.size()/6);
    std::cout << "vert size is " << this->vertices.size() << "\n";
    
    //TODO(darius) UB and stuff
    for (int i = 0; i < verticesRaw.size(); i += 6)
    {
        vertices[i / 6] = {};
        vertices[i / 6].Position = glm::vec3(verticesRaw[i], verticesRaw[i + 1], verticesRaw[i + 2]);
        vertices[i / 6].Normal = glm::vec3(verticesRaw[i + 3], verticesRaw[i + 4], verticesRaw[i + 5]);
        //vertices[i / 7].TexCoords = glm::vec2(verticesRaw[i + 5], verticesRaw[i + 6]);
    }

    this->indices = indicesRaw;
    this->textures = {};

    this -> setupMesh();
}

void CubeMesh::DrawRaw(Shader& shader, size_t sdrp)
{
    glm::mat4 model = glm::mat4(1.0f);
    glm::vec3 pos = { 3,3,3 };
    glm::vec3 scale = { 1,1,1 };

    model = glm::translate(model, pos);
    model = glm::scale(model, scale);

    shader.setMat4("model", model);
    vao.bind();
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);//IMPORTANT
}
