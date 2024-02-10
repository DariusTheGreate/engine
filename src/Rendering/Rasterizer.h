#pragma once
#include <memory>
#include <Rendering/Texture.h>
#include <Rendering/Mesh.h>
#include <Core/utility.h>

class Rasterizer
{
public:
	Rasterizer(size_t w, size_t h) : W(w), H(h)
	{
		//buff = std::make_unique_for_overwrite<unsigned char[]>(W*H);
		buff = new unsigned char[W*H];//allocators or smart poiners
		std::memset(buff, 0, W*H);
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
		return &buff[i * H + j];
	}

	void set1(const glm::vec2& p, const int& w1)
	{
		buff[(int)p.x * H + (int)p.y] = w1;
	}

	void set3(const glm::vec2& p, const int& w1, const int& w2, const int& w3)
	{
		set1({p.x, p.y}, w1);
		set1({p.x+1, p.y}, w1);
		set1({p.x+2, p.y}, w1);
	}

	void trySet3(const glm::vec2& p, const int& w1, const int& w2, const int& w3)
	{
		if(p.x * H + W >= H*W)
			return;

		set1({p.x, p.y}, w1);
		set1({p.x+1, p.y}, w1);
		set1({p.x+2, p.y}, w1);
	}

	size_t getW()
	{
		return W;
	}

	size_t getH()
	{
		return H;
	}

	float orient2d(const glm::vec2& a, const glm::vec2& b, const glm::vec2& c)
	{
	    return (b.x-a.x)*(c.y-a.y) - (b.y-a.y)*(c.x-a.x);
	}

	void drawTri(const Triangle& t) 
	{
		drawTri(t.x, t.y, t.z);
	}

	void drawTri(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2)
	{
	    float minX = vmin(v0.x, v1.x, v2.x);
	    float minY = vmin(v0.y, v1.y, v2.y);
	    float maxX = vmax(v0.x, v1.x, v2.x);
	    float maxY = vmax(v0.y, v1.y, v2.y);

	    minX = vmax(minX, 0);
	    minY = vmax(minY, 0);
	    maxX = vmin(maxX, W - 1);
	    maxY = vmin(maxY, H - 1);

	    glm::vec2 p;
	    for (p.y = minY; p.y <= maxY; p.y++) {
	        for (p.x = minX; p.x <= maxX; p.x++) {
	            // Determine barycentric coordinates
	            int w0 = orient2d(v1, v2, p);
	            int w1 = orient2d(v2, v0, p);
	            int w2 = orient2d(v0, v1, p);

	            // If p is on or inside all edges, render pixel.
	            if (w0 >= 0 && w1 >= 0 && w2 >= 0)
	                set3(p, w0, w1, w2);           
	        }
	    }
	}

	void printBuff()
	{
	    ImageUtils::printImageRawData(buff, W*H);
	}

	void saveBuffToImage()
	{
		ImageUtils::saveImageRawData("buff.jpg", W, H, buff);
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