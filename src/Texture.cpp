#include "Texture.h"

Texture::Texture()
{
	generate();
}

Texture::Texture(const std::string& path_in, unsigned int internalFormat, unsigned int format) : path(path_in) {
	stbi_set_flip_vertically_on_load(true);

	int width, height, nrChannels;
	unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);

	generate();

	bind();

	if (data) {
		imageTexture(internalFormat, width, height);

		filters();

		//OpenglWrapper::GenerateMipmap();
	}

	stbi_image_free(data);

	Width = width;
	Height = height;
}

void Texture::generate()
{
	OpenglWrapper::GenerateTextures(&texture);
}

void Texture::imageTexture(int format, unsigned int W, unsigned int H, const void* data)
{
	OpenglWrapper::ImageTexture(format, W, H, (unsigned char*)data);

	Width = W;
	Height = H;
}

void Texture::activate(GLenum unit, GLenum target) {
	OpenglWrapper::ActivateTexture(unit);
	OpenglWrapper::BindTexture(static_cast<int>(texture), target);
}

void Texture::bind(GLenum target)
{
	OpenglWrapper::BindTexture(static_cast<int>(texture), target);
}

void Texture::unbind(GLenum target)
{
	//glBindTexture(target, 0);
	OpenglWrapper::UnbindTexture();
}

void Texture::filters()
{
	glTexParameteri(target, GL_TEXTURE_MIN_FILTER, minFilter);
	glTexParameteri(target, GL_TEXTURE_MAG_FILTER, magFilter);

	glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

unsigned int Texture::get_texture() {
	return texture;
}

unsigned int* Texture::get_texture_ptr()
{
	return &texture;
}

std::string Texture::get_type() {
	return type;
}

void Texture::set_type(std::string t)
{
	type = t;
}

std::string Texture::get_path() {
	return path;
}

