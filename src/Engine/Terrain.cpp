#define _WIN32_WINNT 0x0601
#include <Windows.h>
#include <Engine/Terrain.h>
#include <Rendering/Renderer.h>
#include <Core/Printer.h>
#include <Rendering/Texture.h>

#include <string>

Terrain::Terrain(std::string heightMap) 
{
    stbi_set_flip_vertically_on_load(true);
    ImageUtils::ImageLoaderPtr imageLoader(heightMap);

    float yScale = 64.0f / 256.0f, yShift = 16.0f;
    int rez = 1;
    unsigned bytePerPixel = imageLoader.numOfChan;
    
    for(int i = 0; i < imageLoader.H; i++)
    {
        for(int j = 0; j < imageLoader.W; j++)
        {
            unsigned char* pixelOffset = imageLoader.data + (j + imageLoader.W* i) * bytePerPixel;
            unsigned char y = pixelOffset[0];

            vertices.push_back( -imageLoader.H/2.0f + imageLoader.H*i/(float)imageLoader.H);
            vertices.push_back( (int) y * yScale - yShift);
            vertices.push_back( -imageLoader.W/2.0f + imageLoader.W*j/(float)imageLoader.W);
        }
    }

    for(size_t i = 0; i < imageLoader.H-1; i += rez)
    {
        for(size_t j = 0; j < imageLoader.W; j += rez)
        {
            for(size_t k = 0; k < 2; k++)
            {
                indices.push_back(static_cast<unsigned int&&>((j + static_cast<size_t>(imageLoader.W) * (i + k*rez))));
            }
        }
    }

    numStrips = (imageLoader.H-1)/rez;
    numTrisPerStrip = (imageLoader.W/rez)*2-2;

    //TODO(darius) create Mesh for it 
    terrainVAO.init();
    terrainVAO.bind();

    terrainVBO.init();
    terrainVBO.bind(vertices.size() * sizeof(float), &vertices[0]);
    terrainVBO.setVAO(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    terrainEBO.init();
    terrainEBO.bind(indices.size() * sizeof(unsigned), &indices[0]);
    auto textID = ImageUtils::TextureFromFile("checkerboard.png", GameState::engine_path + "textures"); 

    texture = Texture(textID, "checkerboard.png", "texture_diffuse");
}

void Terrain::draw()
{
    Renderer::shaderLibInstance->stage = ShaderLibrary::STAGE::TERRAIN;
    Shader sv = Renderer::shaderLibInstance->getCurrShader();
    glUseProgram(sv.getProgram());	

    Object tmp{"terrainTmp"};
    tmp.getTransform().setScale({0.01,0.01,0.01});
    Renderer::shaderLibInstance->shaderRoutine(&tmp);

    OpenglWrapper::SetShaderInt(sv.getShader(), "texture_diffuse1", 0);
    OpenglWrapper::ActivateTexture(GL_TEXTURE0);
    OpenglWrapper::BindTexture(static_cast<int>(texture.get_texture()));

	terrainVAO.bind();

    for(size_t strip = 0; strip < numStrips; strip++)
    {
        glDrawElements(GL_TRIANGLE_STRIP, numTrisPerStrip+2, GL_UNSIGNED_INT, (void*)(sizeof(unsigned) * (numTrisPerStrip+2) * strip));
    }
}
