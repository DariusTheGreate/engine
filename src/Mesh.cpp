#include "Mesh.h"
#include <OpenglWrapper.h>
#include <Renderer.h>
#include <Printer.h>

#include <ranges>
#include <algorithm>
#include <glm/glm.hpp>
#include <limits>

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

void Mesh::updateAABB(Transform tr)
{
    //Timer t(1);
    //TODO(darius) optimise here
    glm::vec3 aabbNewCenter = tr.matrix * glm::vec4(((aabb.min + aabb.max) * 0.5f), 1.0f); 
    if(aabb.size != tr.getScale() || aabb.center != aabbNewCenter)
        calculateAabb(tr);
}

void Mesh::Draw(Shader& shader, int instancedAmount)
{
    //std::unique_lock<std::mutex>(draw_mutex);
    
    //Timer t(true);
    prepareTextures(shader);

    vao.bind();
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
    std::cout << "--------------------------\n";

    shader.setInt(("texture_diffuse_was_set"), 667);
    shader.setInt(("texture_normal_was_set"), 667);
    shader.setInt(("texture_specular_was_set"), 667);
    shader.setInt(("texture_height_was_set"), 667);

    for (int i = 0; i < textures.size(); i++)
    {
        int number = 0;
        std::string name = textures[i].get_type();
        if (name == "texture_diffuse")
            number = (0);
        else if (name == "texture_normal")
            number = (1);
        else if (name == "texture_specular")
            number = (2);
        else if (name == "texture_height")
            number = (3);

        std::cout << (name + std::to_string(number)) << "\n";
        std::cout << "texture: " << textures[i].get_path() << " " << textures[i].get_type() << "\n";

        //OpenglWrapper::SetShaderInt(shader.getShader(), (name + "_was_set").c_str(), 666);
        shader.setInt((name + "_was_set"), 666);
        OpenglWrapper::ActivateTexture(GL_TEXTURE0 + number);
        OpenglWrapper::BindTexture(static_cast<int>(textures[i].get_texture()));
        //OpenglWrapper::BindTexture(Renderer::shaderLibInstance->depthMap);
    }

    std::cout << "--------------------------\n";

    /*OpenglWrapper::SetShaderInt(shader.getShader(), "depthMap", 16);//NOTE(darius) now depthMap is at binmding 16(works on glsl version 420)
    OpenglWrapper::ActivateTexture(GL_TEXTURE0 + 16);
    OpenglWrapper::BindTexture(Renderer::shaderLibInstance->depthMap);
    */
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
    //println("aab update");
    //glm::vec3 vertex = glm::vec3(vertices[indices[0]].Position);
    glm::vec3 vmin = glm::vec3(FLT_MAX, FLT_MAX, FLT_MAX);//vertex;
    glm::vec3 vmax = glm::vec3(FLT_MIN, FLT_MIN, FLT_MIN);//vertex;

    /*for (size_t i = 0; i < indices.size(); ++i) {
        vertex = glm::vec3(vertices[indices[i]].Position);
        vmin = glm::length(vmin) < glm::length(vertex) ? vmin : vertex;
        vmax = glm::length(vmax) > glm::length(vertex) ? vmax : vertex;
    }
    */

    using std::min;
    using std::max;

    /*for (auto& vertex : vertices)
    {
        vmin.x = min(vmin.x, vertex.Position.x);
        vmin.y = min(vmin.y, vertex.Position.y);
        vmin.z = min(vmin.z, vertex.Position.z);

        vmax.x = max(vmax.x, vertex.Position.x);
        vmax.y = max(vmax.y, vertex.Position.y);
        vmax.z = max(vmax.z, vertex.Position.z);
    }
    */

    glm::vec3 vertex = glm::vec3(vertices[0].Position);
    vmin = vertex;
    vmax = vertex;

    for (size_t i = 0; i < vertices.size(); ++i) {
        vertex = glm::vec3(vertices[i].Position);

        vmin.x = min(vmin.x, vertex.x);
        vmin.y = min(vmin.y, vertex.y);
        vmin.z = min(vmin.z, vertex.z);

        vmax.x = max(vmax.x, vertex.x);
        vmax.y = max(vmax.y, vertex.y);
        vmax.z = max(vmax.z, vertex.z);
    }

    //println("min ", vmin);
    //println("max ", vmax);

    /*glm::mat4 m = glm::mat4(1.0f);

    m = glm::scale(m, tr.getScale());

    glm::vec4 vmin4 = m * glm::vec4{vmin.x, vmin.y, vmin.z,0};
    glm::vec4 vmax4 = m * glm::vec4{vmax.x, vmax.y, vmax.z,0};

    vmin = glm::vec3{vmin4.x, vmin4.y, vmin4.z};
    vmax = glm::vec3{vmax4.x, vmax4.y, vmax4.z};
    */

    auto sclMat = glm::scale(tr.matrix, {0.1,0.1,0.1});//for space scaling and depth buffer stuff

    aabb.min = sclMat * glm::vec4(vmin.x, vmin.y, vmin.z, 1.0f);
    aabb.max = sclMat * glm::vec4(vmax.x, vmax.y, vmax.z, 1.0f);
    aabb.center = glm::vec4(((aabb.min + aabb.max) * 0.5f), 1.0f);

    //println(aabb.min, aabb.max);

    //println("we geet ", vmin, vmax, aabb.center);
    //std::cout << aabb.center << "\n";

    aabb.size = (aabb.max - aabb.min);// * 0.5f;
    //aabb.size = {tr.getScale().x, tr.getScale().y, tr.getScale().z};//(vmax - aabb.center);

    //aabb.size *= glm::vec3{0.1,0.1,0.1};
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
