#pragma once
#include <stb_image.h>
#include <string>
#include <glad/glad.h> 
#include <GLFW/glfw3.h>

class Texture
{
public:
	Texture(const std::string& path, unsigned int internalFormat, unsigned int format) {
		stbi_set_flip_vertically_on_load(true);
		int width, height, nrChannels;
		unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		if (data) {
			glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		stbi_image_free(data);
	}

	void activate(GLenum unit, GLenum target = GL_TEXTURE_2D) {
		glActiveTexture(unit);
		glBindTexture(target, texture);
	}
private:
	unsigned int texture;

};

