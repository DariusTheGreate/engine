#pragma once

#include <OpenglWrapper.h>

//NOTE(darius) it very usefull on mobile. cause its expensive to switch stuf there, 
//also it will triger branch predictor for switch instead of card predictor(if it even exist)

class GraphicsStateCache 
{
public:
    void SetDepthTesting(bool enable);

    void EnableDepthTesting();

    void SetCulling(bool enable);

    void SetBlending(bool enable);

    void SetFaceOfCulling(GLenum face);

    void SetPolygonMode(GLenum mode);

    //basicly useless but will see
    void SwitchShader(unsigned int ID);

private:
    bool depthTesting;
    bool blending;
    bool culling;

    GLenum frontFaceOfCulling;
    GLenum polygonMode;

    unsigned int currentShaderID;//NOTE(darius) store in shader library?
};

