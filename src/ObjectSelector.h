#pragma once

#include <OpenglWrapper.h>
#include <iostream>

class ObjectSelector
{
public:
    ObjectSelector() = default;

    ObjectSelector(int W, int H)
    {
        OpenglWrapper::GenerateFrameBuffers(&pick_fbo);
        OpenglWrapper::BindFrameBuffer(pick_fbo);
        OpenglWrapper::GenerateTextures(&pick_texture);
        OpenglWrapper::BindTexture(pick_texture);
        OpenglWrapper::ImageTexture(GL_DEPTH_COMPONENT, W, H, nullptr, GL_TEXTURE_2D, GL_FLOAT);
        OpenglWrapper::ImageFrameBuffer(pick_texture);
        OpenglWrapper::UnbindTexture();
        OpenglWrapper::UnbindFrameBuffer();
    } 
    
    void ReadPixel(int x, int y)
    {
        //std::cout << x << "|" << y << "\n";
        OpenglWrapper::BindFrameBuffer(pick_fbo);
        OpenglWrapper::ReadBuffer();
        int pixel[4] = {0, 0, 0, 0};
        OpenglWrapper::ReadPixels(x, y, &pixel);
        OpenglWrapper::ReadZeroBuffer();
        //std::cout << pixel[0] << " " << pixel[1] << "\n";
        OpenglWrapper::UnbindFrameBuffer();
    }

private:
    size_t pick_fbo = 0;
    size_t pick_texture = 0;
};

