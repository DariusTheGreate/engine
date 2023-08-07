#include <RenderQuad.h>

RendererQuad::RendererQuad() : qv(GameState::engine_path + "shaders/quadShader.glsl", GL_VERTEX_SHADER), 
    qf(GameState::engine_path + "shaders/quadShaderFragment.glsl", GL_FRAGMENT_SHADER)
{
    qv.compile();
    qf.compile();
    qv.link(qf);

    /*vao.init();
    vbo.init();
    vao.bind();
    vbo.bind(vertices.size() * sizeof(float), &vertices[0]);
    vbo.vboEnableVertexAttribArray(0);
    vbo.setVAO(0,2,GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);

    vbo.vboEnableVertexAttribArray(1);
    vbo.setVAO(1,2,GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 + sizeof(float)));*/

    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
}

void RendererQuad::DrawQuad(unsigned int screenTexture)
{
    /*glUseProgram(qv.getProgram());
    vao.bind();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, screenTexture);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    */

    glUseProgram(qv.getProgram());

    glBindVertexArray(quadVAO);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, screenTexture);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void RendererQuad::DrawQuad(FrameBuffer& buff) 
{
    if(buff.getTextures().size() > 0)
        DrawQuad((unsigned int)buff.getTexture().get_texture());
}

void RendererQuad::DrawQuad(FrameBuffer& buff, unsigned int textureNumber) 
{
    DrawQuad(buff.getTextureAt(textureNumber).get_texture());
}

void RendererQuad::DrawQuadToBindedTexture(Shader s) 
{
    glUseProgram(s.getProgram());

    glBindVertexArray(quadVAO);

    //glActiveTexture(GL_TEXTURE0);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void RendererQuad::bindQuadVAO() 
{
    glBindVertexArray(quadVAO);
}

void RendererQuad::drawArrays() 
{
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

