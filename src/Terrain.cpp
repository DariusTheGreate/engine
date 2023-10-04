#include <Terrain.h>
#include <Renderer.h>
#include <Printer.h>
#include <Texture.h>

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

    for(unsigned i = 0; i < imageLoader.H-1; i += rez)
    {
        for(unsigned j = 0; j < imageLoader.W; j += rez)
        {
            for(unsigned k = 0; k < 2; k++)
            {
                indices.push_back(j + imageLoader.W* (i + k*rez));
            }
        }
    }

    numStrips = (imageLoader.H-1)/rez;
    numTrisPerStrip = (imageLoader.W/rez)*2-2;

    terrainVAO.init();
    terrainVAO.bind();

    terrainVBO.init();
    terrainVBO.bind(vertices.size() * sizeof(float), &vertices[0]);
    terrainVBO.setVAO(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    terrainEBO.init();
    terrainEBO.bind(indices.size() * sizeof(unsigned), &indices[0]);
}

void Terrain::draw()
{
	//TODO(darius) make it terrain shader
	//Shader sv = Renderer::shaderLibInstance->getTerrainShader();

    Renderer::shaderLibInstance->stage = ShaderLibrary::STAGE::TERRAIN;
    Shader sv = Renderer::shaderLibInstance->getCurrShader();
    glUseProgram(sv.getProgram());	

    Object tmp{"terrainTmp"};
    tmp.getTransform().setScale({0.1,0.1,0.1});
    Renderer::shaderLibInstance->shaderRoutine(&tmp);

	terrainVAO.bind();
    for(unsigned strip = 0; strip < numStrips; strip++)
    {
        glDrawElements(GL_TRIANGLE_STRIP, numTrisPerStrip+2, GL_UNSIGNED_INT, (void*)(sizeof(unsigned) * (numTrisPerStrip+2) * strip));
    }
}