#pragma once
#include <vector>

#include <Texture.h>

class FrameBuffer
{
public:
    FrameBuffer(bool is_static_initialized = false);

	void AttachTexture(unsigned int W, unsigned int H, int numOfColorAttachments = 1);

	void AttachMultisampledTexture(unsigned int W, unsigned int H);

    void Generate();

	void Bind();

	void Unbind();

    void Blit();

    void Blit(FrameBuffer& buff1, FrameBuffer& buff2);

    void ClearBlit();

    void SetImage(int i);

	Texture& getTexture();
    Texture& getTextureAt(int i);
    std::vector<Texture>& getTextures();

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

struct BinderPointer 
{
    BinderPointer(FrameBuffer* buff_in) : buff(buff_in)
    {
        buff->Bind();
    }

    //NOTE(darius) dangle?
	BinderPointer(FrameBuffer& buff_in) : buff(&buff_in)
    {
        buff->Bind();
    }

    ~BinderPointer() 
    {
        buff->Unbind();
    }

    FrameBuffer* buff;
};
