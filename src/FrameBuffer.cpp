#include <FrameBuffer.h>
#include <Renderer.h>

FrameBuffer::FrameBuffer()
{
    OpenglWrapper::GenerateFrameBuffers((size_t*)(&ID));
}

void FrameBuffer::AttachTexture(unsigned int W, unsigned int H, int numOfColorAttachments)
{
    Width = W;
    Height = H;

    Bind();

    for (int i = 0; i < numOfColorAttachments; ++i)
    {
        Texture ti;
        ti.bind();
        ti.imageTexture(GL_RGB, Width, Height, nullptr);
        ti.filters();

		OpenglWrapper::ImageFrameBuffer((unsigned int)ti.get_texture(), GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D);

        textures.push_back(ti);
    }

    unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(numOfColorAttachments, attachments);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;

    //Unbind();
}

void FrameBuffer::AttachMultisampledTexture(unsigned int W, unsigned int H)
{
    multisampled = true;

	Width = W;
    Height = H;

    Bind();

    Texture texture;
    texture.bind(GL_TEXTURE_2D_MULTISAMPLE);
    OpenglWrapper::ImageMultisampleTexture(GL_RGB, Width, Height, 4);
    OpenglWrapper::ImageFrameBuffer((unsigned int)texture.get_texture(), GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE);

    textures.push_back(texture);
    
	RenderBuffer renderBuffer = RenderBuffer(Width, Height);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
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
    return textures[0];
}

Texture& FrameBuffer::getTextureAt(int i)
{
    return textures[i];
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
