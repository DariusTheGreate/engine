#pragma once
#include <stb_image.h>
#include <string>
#include <OpenglWrapper.h>

class Texture
{
public:
	Texture() = default;
	Texture(const Texture& t) = default;

	Texture(unsigned int texture_in, const std::string& path_in, const std::string& type_in) : texture(texture_in), path(path_in), type(type_in) {}

	Texture(const std::string& path_in, unsigned int internalFormat, unsigned int format);

	void activate(GLenum unit, GLenum target = GL_TEXTURE_2D);
	
	size_t get_texture();

	std::string get_type();
	
	std::string get_path();

private:
	size_t texture;
	std::string type;
	std::string path;
};

