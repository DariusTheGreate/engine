#include <frameBuffer.h>

FrameBuffer::FrameBuffer()
{
    OpenglWrapper::GenerateFrameBuffers((size_t*)(&ID));
}

void FrameBuffer::AttachTexture(unsigned int W, unsigned int H, bool multisample)
{
    Width = W;
    Height = H;

    Bind();

    if (multisample)
    {
        texture.bind(GL_TEXTURE_2D_MULTISAMPLE);
        OpenglWrapper::ImageMultisampleTexture(GL_RGB, Width, Height, 4);
        OpenglWrapper::ImageFrameBuffer((unsigned int)texture.get_texture(), GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE);
    }
    else
    {
        texture.bind();
        OpenglWrapper::ImageTexture(GL_RGB, Width, Height, 0);
        texture.filters();
        OpenglWrapper::ImageFrameBuffer((unsigned int)texture.get_texture(), GL_COLOR_ATTACHMENT0);
    }

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;

    //Unbind();
}

void FrameBuffer::Bind()
{
    OpenglWrapper::BindFrameBuffer(ID, target);
}

void FrameBuffer::Unbind()
{
    OpenglWrapper::UnbindFrameBuffer(target);
}

void FrameBuffer::Blit()
{
	glBlitFramebuffer(0, 0, Width, Height, 0, 0, Width, Height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
}

Texture& FrameBuffer::getTexture()
{
    return texture;
}

void FrameBuffer::setTaget(GLenum newTarget)
{
    target = newTarget;
}

GLenum FrameBuffer::getTarget()
{
    return target;
}

unsigned int FrameBuffer::getID()
{
    return ID;
}
