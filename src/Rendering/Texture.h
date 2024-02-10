#pragma once

#include <stb_image.h>


#include <string>
#include <Rendering/OpenglWrapper.h>
#include <Core/Printer.h>

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
	        //print("Loaded Texture\n");
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

	static void printImageRawData(unsigned char* data, size_t sz)
	{
		//size_t i = 0;	
		//while(data[i] != '\0'){

		for(size_t i = 0; i < sz; ++i){
			print((unsigned int)(data[i]), "|");
		}
	}

	static void saveImageRawData(const char* filename, int32_t w, int32_t h, uint8_t* data);

	static unsigned int TextureFromFile(const char* filename, bool rotateTextureOnLoad = false)
	{
		unsigned int textureID = 0;
	    OpenglWrapper::GenerateTextures(&textureID);

	    stbi_set_flip_vertically_on_load(rotateTextureOnLoad);
	    int width, height, nrComponents;
	    unsigned char* data = stbi_load(filename, &width, &height, &nrComponents, 0);
	    //ImageUtils::printImageRawData(data, width * height);
	    if (data)
	    {
	        GLenum format = GL_RGBA;
	        if (nrComponents == 1)
	            format = GL_RED;
	        else if (nrComponents == 3)
	            format = GL_RGB;
	        
	        OpenglWrapper::BindTexture(static_cast<int>(textureID));
	        OpenglWrapper::ImageTexture(format, width, height, data);
	        glGenerateMipmap(GL_TEXTURE_2D);

	        OpenglWrapper::TextureParameter(GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
	        OpenglWrapper::TextureParameter(GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
	        OpenglWrapper::TextureParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	        OpenglWrapper::TextureParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	        stbi_image_free(data);
	    }
	    else
	    {
	        std::cout << "Texture failed to load at path: " << filename << std::endl;
	        stbi_image_free(data);
	    }

	    return static_cast<unsigned int>(textureID);
	}

	static unsigned int TextureFromFile(const std::string& path, const std::string& directory, bool rotateTextureOnLoad = false)
	{
	    std::string filename = path;
	    filename = directory + '/' + filename;
	    return TextureFromFile(filename.c_str(), rotateTextureOnLoad);	
	}
};

//TODO(darius) refactor invariants
class Texture
{
public:
	Texture();

	Texture(const Texture& t) = default;

	Texture(unsigned int texture_in, const std::string& path_in, const std::string& type_in) : texture(texture_in), path(path_in), type(type_in) {}

	Texture(const std::string& path_in, unsigned int internalFormat = GL_RGBA, unsigned int format = GL_RGBA);

	Texture(unsigned char* data, size_t W, size_t H, unsigned int format = GL_RGBA);

	void generate();
	
	void imageTexture(int format, size_t W, size_t H, const void* data = nullptr);

	void activate(GLenum unit, GLenum target = GL_TEXTURE_2D);

	void bind(GLenum target = GL_TEXTURE_2D);

	void unbind(GLenum target = GL_TEXTURE_2D);

	void resize(unsigned int width, unsigned int height);

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

	size_t Width = 0;
	size_t Height = 0;

	std::string type;
	std::string path;
};

