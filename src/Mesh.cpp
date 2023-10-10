#include "Mesh.h"
#include <OpenglWrapper.h>
#include <Renderer.h>
#include <Printer.h>

#include <ranges>
#include <algorithm>
#include <glm/glm.hpp>

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures) 
    : vertices(std::move(vertices)), indices(std::move(indices)), textures(std::move(textures))
{
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

std::vector<Vertex>& Mesh::getVerticesRef()
{
    return vertices; 
}

std::vector<unsigned int> Mesh::getIndices()
{
    return indices;
}

std::vector<unsigned int>& Mesh::getIndicesRef()
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

void Mesh::setTexture(const std::string& path, const std::string& name)
{
    //TODO(darius) make it single texture
    std::string fullPath;
    fullPath.reserve(path.size() + name.size() + 1);
    fullPath.append(path);
    fullPath.append("/");
    fullPath.append(name);

    Texture texture(TextureFromFile(name.c_str(), path.c_str(), false, false), fullPath , "texture_diffuse");
    textures.push_back(texture);
}

void Mesh::setTexture(const std::string& path)
{
    //TODO(darius) make it single texture
    Texture texture(TextureFromFile(path.c_str(), false, false), path , "texture_diffuse");
    textures.push_back(texture);
}

void Mesh::clearMesh() 
{
    vao.deleteVAO();
    vbo.deleteVBO();
    ebo.deleteEBO();
}

VAO Mesh::getVao()
{
    return vao;
}

DrawMode Mesh::getDrawMode()
{
    return mode;
}

MeshType Mesh::getType()
{
    return type;
}

MeshAABB Mesh::getAABB()
{
    return aabb;
}

void Mesh::Draw(Shader& shader, int instancedAmount)
{
    //std::unique_lock<std::mutex>(draw_mutex);
    
    prepareTextures(shader);

    //TODO(darius) perfomance issues?
    if(mode == DrawMode::DRAW_AS_ARRAYS)
		glDrawArrays(GL_TRIANGLES, 0, vertices.size());
    else if(mode == DrawMode::DRAW_AS_INSTANCE)
         glDrawElementsInstanced(
        GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0, instancedAmount
    );
    else            
        OpenglWrapper::DrawElements(static_cast<int>(indices.size()));

    //OpenglWrapper::UnbindVAO();

    Renderer::drawCallsCount++;
}

void Mesh::prepareTextures(Shader& shader)
{
    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;
    unsigned int normalNr = 1;
    unsigned int heightNr = 1;

    vao.bind();

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
    glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, m_BoneIDs));

    OpenglWrapper::EnableAttribute(6);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_Weights));
    glBindVertexArray(0);
}

void Mesh::printVertices()
{
    std::cout << "Verts:\n";
    for(int i = 0; i < vertices.size(); ++i)
    {
        std::cout << vertices[i].Position.x << " " << vertices[i].Position.y << " " << vertices[i].Position.z << "\n"; 
    } 

    std::cout << "Inds:\n";
    for(int i = 0; i < indices.size(); ++i)
    {
        std::cout << indices[i] << "\n";
    } 
}

void Mesh::addVerticesBath(Mesh& batchee, glm::vec3 shift)
{   
    //NOTE(darius) for some reason it doesnt change runtime state fo mesh. But it changes its vectors, so  if u serialize it - it will work..
    //NOTE(darius) if u create new mesh from this mesh vectors u can go ril fast
    //TODO(darius) it now only batches first mesh from input. Not urgent, but needed to batch all meshes of batchee

    auto verts = batchee.getVertices() | std::ranges::views::take(4);
    std::ranges::for_each(verts, [shift](auto& v){v.Position += shift; });

    int offset = vertices.size();
    vertices.insert(vertices.end(), verts.begin(), verts.end());

    auto indV = batchee.getIndices();

    //std::cout << "offset" << offset << "\n";
    //std::cout << "indices size " << indV.size() << "\n";

    for(auto& i : indV)
    {
        i += offset;
    }

    indices.insert(indices.end(), indV.begin(), indV.begin() + 6);
    //mode = DrawMode::DRAW_AS_ARRAYS;
}

void Mesh::clearBatch(size_t verticesNum, size_t indicesNum)
{
    vertices.erase(vertices.begin() + verticesNum, vertices.end()); 
    indices.erase(indices.begin() + indicesNum, indices.end()); 
}

//TODO(darius) apply scale?
void Mesh::calculateAabb(Transform& tr) 
{
    glm::vec3 vertex = glm::vec3(vertices[indices[0]].Position);
    glm::vec3 vmin = vertex;
    glm::vec3 vmax = vertex;

    for (size_t i = 0; i < indices.size(); ++i) {
        vertex = glm::vec3(vertices[indices[i]].Position);
        vmin = glm::length(vmin) < glm::length(vertex) ? vmin : vertex;
        vmax = glm::length(vmax) > glm::length(vertex) ? vmax : vertex;
    }

    /*glm::mat4 m = glm::mat4(1.0f);

    m = glm::scale(m, tr.getScale());

    glm::vec4 vmin4 = m * glm::vec4{vmin.x, vmin.y, vmin.z,0};
    glm::vec4 vmax4 = m * glm::vec4{vmax.x, vmax.y, vmax.z,0};

    vmin = glm::vec3{vmin4.x, vmin4.y, vmin4.z};
    vmax = glm::vec3{vmax4.x, vmax4.y, vmax4.z};
    */

    aabb.min = vmin;
    aabb.max = vmax;

    aabb.center = tr.matrix * glm::vec4(((vmin + vmax) * 0.5f), 1.0f);

    aabb.size = {tr.getScale().x, tr.getScale().y, tr.getScale().z};//(vmax - aabb.center);
}

std::vector<unsigned int> Mesh::generateLOD()
{   
    std::vector<unsigned int> indicesGenerated; 

    for(int i = 0; i < indices.size(); ++i)
    {
        if(i % 2 == 0)
            indicesGenerated.push_back(indices[i]); 
    }

    return indicesGenerated;
}

bool MeshAABB::isOnFrustum(Frustum& camFrustum, Transform& transform) 
{
    //NOTE(darius) TEMPO while scale dont work
    //if(transform.getScale() != glm::vec3{1,1,1})
    //    return true;

    //glm::vec3 globalCenter{ transform.matrix * glm::vec4(center, 1.f) };
    //glm::vec3 scl = transform.getScale();

    //MeshAABB globalAABB(globalCenter, scl.x, scl.y, scl.z);

    return (isOnOrForwardPlane(camFrustum.leftFace) &&
        isOnOrForwardPlane(camFrustum.rightFace) &&
        isOnOrForwardPlane(camFrustum.topFace) &&
        isOnOrForwardPlane(camFrustum.bottomFace) &&
        isOnOrForwardPlane(camFrustum.nearFace) &&
        isOnOrForwardPlane(camFrustum.farFace));
};
