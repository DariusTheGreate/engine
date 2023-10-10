#pragma once
#include <stb_image.h>
#include <string>
#include <OpenglWrapper.h>
#include <Printer.h>

struct ImageUtils
{

	struct ImageLoaderPtr
	{
		ImageLoaderPtr(std::string path)
		{
			data = ImageUtils::loadStbImage(path, &W, &H, &numOfChan);
		}

		~ImageLoaderPtr()
		{
		    ImageUtils::freeImage(data);
		}

		int H = 0;
		int W = 0;
		int numOfChan = 0;

		unsigned char* data = nullptr;
	};

	static unsigned char* loadStbImage(std::string_view path, int* W, int* H, int* numOfChan)
	{
		unsigned char *data = stbi_load(std::string(path).c_str(), W, H, numOfChan, 0);
	    if (data)
	    {
	        print("Loaded Texture\n");
	    }
	    else
	    {
	        print("Failed to load Texture\n");
	        return nullptr;
	    }

	    return data;
	}

	static void freeImage(unsigned char* data)
	{
		stbi_image_free(data);
	}
};

class Texture
{
public:
	Texture();

	Texture(const Texture& t) = default;

	Texture(unsigned int texture_in, const std::string& path_in, const std::string& type_in) : texture(texture_in), path(path_in), type(type_in) {}

	Texture(const std::string& path_in, unsigned int internalFormat = GL_RGBA, unsigned int format = GL_RGBA);

	void generate();
	
	void imageTexture(int format, unsigned int W, unsigned int H, const void* data = nullptr);

	void activate(GLenum unit, GLenum target = GL_TEXTURE_2D);

	void bind(GLenum target = GL_TEXTURE_2D);

	void unbind(GLenum target = GL_TEXTURE_2D);

	void filters();
	
	unsigned int get_texture();

	//NOTE(darius) danger?
	unsigned int* get_texture_ptr();

	std::string get_type();

	void set_type(std::string t);
	
	std::string get_path();

private:
	unsigned int texture = 0;

	GLenum target = GL_TEXTURE_2D;
	GLenum format = GL_RGBA;
	GLenum minFilter = GL_NEAREST;//GL_LINEAR_MIPMAP_LINEAR;
	GLenum magFilter = GL_NEAREST;

	unsigned int Width = 0;
	unsigned int Height = 0;

	std::string type;
	std::string path;
};

