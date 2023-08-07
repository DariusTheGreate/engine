#include <FrameBuffer.h>
#include <Renderer.h>

FrameBuffer::FrameBuffer(bool is_static_initialized)
{
    if (!is_static_initialized)
        Generate();
}

void FrameBuffer::AttachTexture(unsigned int W, unsigned int H, int numOfColorAttachments)
{
    Width = W;
    Height = H;

    Bind();
    setTaget(GL_FRAMEBUFFER);

    for (int i = 0; i < numOfColorAttachments; ++i)
    {
        Texture ti;
        ti.bind();
        ti.imageTexture(GL_RGB, Width, Height, nullptr);
        ti.filters();

		OpenglWrapper::ImageFrameBuffer((unsigned int)ti.get_texture(), GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D);

        textures.push_back(ti);
    }

    //TODO(darius) generalize this
    //TODO(darius) theres no depth buffer here

    unsigned int rboDepth;
    glGenRenderbuffers(1, &rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, Width, Height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

    unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
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

void FrameBuffer::Generate()
{
		OpenglWrapper::GenerateFrameBuffers((size_t*)(&ID));
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
    ClearBlit();
}

void FrameBuffer::Blit(FrameBuffer& buff1, FrameBuffer& buff2)
{
    buff1.setTaget(GL_READ_FRAMEBUFFER);
    buff1.Bind();

    buff2.setTaget(GL_DRAW_FRAMEBUFFER);
    buff2.Bind();

    buff2.Blit();
}

void FrameBuffer::ClearBlit()
{
	OpenglWrapper::UnbindFrameBuffer(GL_FRAMEBUFFER);
	OpenglWrapper::DisabelDepthTest();
	OpenglWrapper::ClearScreen({1,1,1});
}

void FrameBuffer::SetImage(int i)
{
    if (i > textures.size() - 1)
        return;
    OpenglWrapper::ImageFrameBuffer((unsigned int)textures[i].get_texture(), GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D);
}

Texture& FrameBuffer::getTexture()
{
    return textures[0];
}

Texture& FrameBuffer::getTextureAt(int i)
{
    return textures[i];
}

std::vector<Texture>& FrameBuffer::getTextures()
{
    return textures;
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

