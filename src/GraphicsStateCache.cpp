#include <GraphicsStateCache.h>

void GraphicsStateCache::SetDepthTesting(bool enable)
{
    if (depthTesting != enable)
    {
        depthTesting = enable;
        if (enable)
            glEnable(GL_DEPTH_TEST);
        else
            glDisable(GL_DEPTH_TEST);
    }
}

void GraphicsStateCache::EnableDepthTesting()
{
    SetDepthTesting(true);
}

void GraphicsStateCache::SetBlending(bool enable)
{
    if (blending != enable)
    {
        blending = enable;
        if(enable)
            glEnable(GL_BLEND);
        else
            glDisable(GL_BLEND);
    }
}

void GraphicsStateCache::SetCulling(bool enable)
{
    if (culling != enable)
    {
        culling = enable;
        if(enable)
            glEnable(GL_CULL_FACE);
        else
            glDisable(GL_CULL_FACE);
    }
}

void GraphicsStateCache::SetFaceOfCulling(GLenum face)
{
    if (frontFaceOfCulling != face)
    {
        frontFaceOfCulling = face;
        glCullFace(face);
    }
}

void GraphicsStateCache::SetPolygonMode(GLenum mode)
{
    if (polygonMode != mode)
    {
        polygonMode = mode;
        glPolygonMode(GL_FRONT_AND_BACK, mode);
    }
}

void GraphicsStateCache::SwitchShader(unsigned int ID)
{
    if (currentShaderID != ID)
    {
        glUseProgram(ID);
        currentShaderID = ID;
    }
}

