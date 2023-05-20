#pragma once

#include <Mesh.h>

class CubeMesh : public Mesh
{
public:
    CubeMesh()
    {
        this->vertices.resize(verticesRaw.size()/3);
        std::cout << "vert size is " << this->vertices.size() << "\n";
        
        //TODO(darius) UB and stuff
        for(int i = 0; i < verticesRaw.size(); i += 3)
        {
            vertices[i / 3] = {};
            vertices[i / 3].Position = glm::vec3(verticesRaw[i], verticesRaw[i+1], verticesRaw[i+2]);
        }

        this->indices = indicesRaw;
        this->textures = {};

        this -> setupMesh();
    }

    void DrawRaw(Shader& shader, size_t sdrp)
    {
        glm::mat4 model = glm::mat4(1.0f);
        glm::vec3 pos = { 3,3,3 };
        glm::vec3 scale = { 1,1,1 };

        model = glm::translate(model, pos);
        model = glm::scale(model, scale);

        shader.setMat4("model", model, sdrp);
        vao.bind();
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);//IMPORTANT
    }

 private:
    std::vector<float> verticesRaw = {
        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,

        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,

         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,

        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,
    };

    std::vector<unsigned int> indicesRaw = {
        0, 1, 3,
        1, 2, 3
    };
};

