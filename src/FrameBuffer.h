#pragma once

#include <Texture.h>

class FrameBuffer
{
public:
    FrameBuffer();

	void AttachTexture(unsigned int W, unsigned int H, bool multisample = false);

	void Bind();

	void Unbind();

    void Blit();

	Texture& getTexture();

	void setTaget(GLenum newTarget);
    GLenum getTarget();

	unsigned int getID();

private:
    unsigned int ID = 0;

    unsigned int Width = 0;
    unsigned int Height = 0;

    GLenum target = GL_FRAMEBUFFER;

    Texture texture;

    bool depthAndStencil = true;
};
