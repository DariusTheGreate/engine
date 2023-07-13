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

	Texture(const std::string& path_in, unsigned int internalFormat, unsigned int format) : path(path_in) {
		stbi_set_flip_vertically_on_load(true);
		int width, height, nrChannels;
		unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
        OpenglWrapper::GenerateTextures(&texture);
        OpenglWrapper::BindTexture(static_cast<int>(texture));
		if (data) {
            OpenglWrapper::ImageTexture(internalFormat, width, height, data);
            OpenglWrapper::GenerateMipmap();
		}
		stbi_image_free(data);
	}

	void activate(GLenum unit, GLenum target = GL_TEXTURE_2D) {
        OpenglWrapper::ActivateTexture(unit);
        OpenglWrapper::BindTexture(static_cast<int>(texture), target);
	}
	
	size_t get_texture() {
		return texture;
	}

	std::string get_type() {
		return type;
	}
	
	std::string get_path() {
		return path;
	}

private:
	size_t texture;
	std::string type;
	std::string path;
};

