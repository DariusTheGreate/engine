#include <Rendering/InstancedRenderer.h>
#include <Rendering/Renderer.h>

void InstancedRenderer::init()
{
    generateBuffer();
    setupBuffer();
}

void InstancedRenderer::setupMesh(Mesh* meshptr)
{
    mesh = meshptr;
}

void InstancedRenderer::generateBuffer()
{
    glGenBuffers(1, &buffer);
}

void InstancedRenderer::setupBuffer()
{
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * instanceMats.size(), &instanceMats[0], GL_STATIC_DRAW);

    mesh->getVao().bind();
 
    std::size_t vec4Size = sizeof(glm::vec4);
    glEnableVertexAttribArray(3); 
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)0);
    glEnableVertexAttribArray(4); 
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(1 * vec4Size));
    glEnableVertexAttribArray(5); 
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(2 * vec4Size));
    glEnableVertexAttribArray(6); 
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(3 * vec4Size));

    glVertexAttribDivisor(3, 1);
    glVertexAttribDivisor(4, 1);
    glVertexAttribDivisor(5, 1);
    glVertexAttribDivisor(6, 1);

    glBindVertexArray(0);
}

void InstancedRenderer::pushMat(glm::mat4 mat)
{
    instanceMats.emplace_back(std::move(mat));
}
