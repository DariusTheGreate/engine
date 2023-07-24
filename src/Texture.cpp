#include "Texture.h"

Texture::Texture()
{
	OpenglWrapper::GenerateTextures(&texture);
}

Texture::Texture(const std::string& path_in, unsigned int internalFormat, unsigned int format) : path(path_in) {
	stbi_set_flip_vertically_on_load(true);
	int width, height, nrChannels;
	unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
	OpenglWrapper::GenerateTextures(&texture);
	OpenglWrapper::BindTexture(static_cast<int>(texture));
	if (data) {
		OpenglWrapper::ImageTexture(internalFormat, width, height, data);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		//OpenglWrapper::GenerateMipmap();
	}
	stbi_image_free(data);
}

void Texture::activate(GLenum unit, GLenum target) {
	OpenglWrapper::ActivateTexture(unit);
	OpenglWrapper::BindTexture(static_cast<int>(texture), target);
}

void Texture::bind(GLenum target)
{
	OpenglWrapper::BindTexture(static_cast<int>(texture), target);
}

void Texture::filters()
{
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

size_t Texture::get_texture() {
	return texture;
}

unsigned int* Texture::get_texture_ptr()
{
	return &texture;
}

std::string Texture::get_type() {
	return type;
}

std::string Texture::get_path() {
	return path;
}

