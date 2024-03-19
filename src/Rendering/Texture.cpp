#include "Texture.h"

#define  __STDC_LIB_EXT1__
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h> 

Texture::Texture()
{
	generate();
}

Texture::Texture(const std::string& path_in, unsigned int internalFormat, unsigned int format) : path(path_in) {
	stbi_set_flip_vertically_on_load(true);

	int width, height, nrChannels;
	//NOTE(darius) internalFormat and nrChannels stuff
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

Texture::Texture(unsigned char* data, size_t W, size_t H, unsigned int format)
{
	generate();	
	bind();

	if(data){
		imageTexture(format, W, H);
		filters();
	}

	Width = W;
	Height = H;
}

void Texture::generate()
{
	OpenglWrapper::GenerateTextures(&texture);
}

void Texture::imageTexture(int format, size_t W, size_t H, const void* data)
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

void Texture::resize(unsigned int width, unsigned int height)
{
	OpenglWrapper::ImageTexture(format, width, height, nullptr);	
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

void ImageUtils::saveImageRawData(const char* filename, int32_t w, int32_t h, uint8_t* data)
{
	stbi_write_jpg(filename, w, h, 1, data, 100);
}

