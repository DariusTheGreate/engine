#pragma once

#include <Rendering/VAO.h>
#include <Rendering/VBO.h>
#include <Rendering/Shader.h>
#include <Rendering/FrameBuffer.h>
#include <Engine/GameState.h>

class RendererQuad 
{
public:
    RendererQuad();

    void DrawQuad(unsigned int screenTexture);

    void DrawQuad(FrameBuffer& buff);

    void DrawQuad(FrameBuffer& buff, unsigned int textureNumber);

    void DrawQuadToBindedTexture(Shader s);

    void bindQuadVAO(); 

    void drawArrays(); 

private:
    VAO vao;
    VBO vbo;
    unsigned int quadVAO;
    unsigned int quadVBO;

    //Shader qv;
    //Shader qf;

    std::vector<float> vertices = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
    // positions   // texCoords
    -1.0f,  1.0f,  0.0f, 1.0f,
    -1.0f, -1.0f,  0.0f, 0.0f,
     1.0f, -1.0f,  1.0f, 0.0f,

    -1.0f,  1.0f,  0.0f, 1.0f,
     1.0f, -1.0f,  1.0f, 0.0f,
     1.0f,  1.0f,  1.0f, 1.0f
	};
};

