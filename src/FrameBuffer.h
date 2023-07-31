#pragma once
#include <vector>

#include <Texture.h>

class FrameBuffer
{
public:
    FrameBuffer();

	void AttachTexture(unsigned int W, unsigned int H, int numOfColorAttachments = 1);

	void AttachMultisampledTexture(unsigned int W, unsigned int H);

	void Bind();

	void Unbind();

    void Blit();

	Texture& getTexture();
    Texture& getTextureAt(int i);

	void setTaget(GLenum newTarget);
    GLenum getTarget();

	unsigned int getID();

private:
    unsigned int ID = 0;

    unsigned int Width = 0;
    unsigned int Height = 0;

    GLenum target = GL_FRAMEBUFFER;

    //Texture texture;

    std::vector<Texture> textures;

    bool depthAndStencil = false;
    bool multisampled = false;
};
