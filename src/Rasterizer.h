#pragma once
#include <memory>
#include <Texture.h>

class Rasterizer
{
public:
	Rasterizer(size_t w, size_t h) : W(w), H(h)
	{
		//buff = std::make_unique_for_overwrite<unsigned char[]>(W*H);
		buff = new unsigned char[W*H];//allocators or smart poiners
	}

	~Rasterizer()
	{
		delete buff;//wrap it into allocator or something
	}

	auto getBuff()
	{
		return buff;
	}

	auto* at(size_t i, size_t j)
	{
		return &buff[i * H + W];
	}

	size_t getW()
	{
		return W;
	}

	size_t getH()
	{
		return H;
	}

	void printBuff()
	{
	    ImageUtils::printImageRawData(buff, W*H);
	}
	
	Texture genTexture()
	{
		return Texture(buff, W, H, GL_RGB);
	}	

private:
	//std::unique_ptr<unsigned char[]> buff = nullptr;
	unsigned char* buff = nullptr;
	size_t W = 0;
	size_t H = 0;
};