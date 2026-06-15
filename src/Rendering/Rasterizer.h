#pragma once
#include <memory>
#include <vector>
#include <limits>
#include <algorithm>
#include <Rendering/Texture.h>
#include <Rendering/Mesh.h>
#include <Core/utility.h>

class Rasterizer
{
public:
	static constexpr float FAR_DEPTH = std::numeric_limits<float>::max();

	Rasterizer(uint32_t w, uint32_t h) : W(w), H(h)
	{
		//buff = std::make_unique_for_overwrite<unsigned char[]>(W*H);
		buff = new unsigned char[W*H];//allocators or smart poiners
		std::memset(buff, 0, W*H);
		depthBuff.assign(size_t(W) * H, FAR_DEPTH);
		colorBuff.assign(size_t(W) * H * 3, 0);
	}

	~Rasterizer()
	{
		delete buff;//wrap it into allocator or something
	}

	auto getBuff()
	{
		return buff;
	}

	auto* at(uint32_t i, uint32_t j)
	{
		return &buff[i * H + j];
	}

	void set1(const glm::vec2& p, const uint32_t& w1)
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

	int orient2d(const glm::vec2& a, const glm::vec2& b, const glm::vec2& c)
	{
	    return int((b.x-a.x)*(c.y-a.y) - (b.y-a.y)*(c.x-a.x));
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

	    minX = vmax(minX, 0.0f);
	    minY = vmax(minY, 0.0f);
	    maxX = vmin(maxX, float(W - 1));
	    maxY = vmin(maxY, float(H - 1));

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
		if(!vizTex)
			vizTex = std::make_unique<Texture>(colorBuff.data(), W, H, GL_RGB);
		else
		{
			vizTex->bind();
			vizTex->imageTexture(GL_RGB, W, H, colorBuff.data());
		}
		return *vizTex;
	}

	void clearDepth()
	{
		std::fill(depthBuff.begin(), depthBuff.end(), FAR_DEPTH);
	}

	float depthAt(uint32_t x, uint32_t y) const
	{
		return depthBuff[size_t(x) * H + y];
	}

	size_t coveredCount() const
	{
		size_t n = 0;
		for (float d : depthBuff)
			if (d < FAR_DEPTH)
				++n;
		return n;
	}

	int rasterizeTriangleDepth(const glm::vec2& v0, const glm::vec2& v1, const glm::vec2& v2,
		float d0, float d1, float d2)
	{
		float triDepth = vmax(d0, d1, d2);

		int minX = std::max(int(vmin(v0.x, v1.x, v2.x)), 0);
		int minY = std::max(int(vmin(v0.y, v1.y, v2.y)), 0);
		int maxX = std::min(int(vmax(v0.x, v1.x, v2.x)), int(W) - 1);
		int maxY = std::min(int(vmax(v0.y, v1.y, v2.y)), int(H) - 1);

		if (minX > maxX || minY > maxY)
			return 0;

		auto edgeAt = [](const glm::vec2& a, const glm::vec2& b, int px, int py) -> long {
			return long((b.x - a.x) * (float(py) - a.y) - (b.y - a.y) * (float(px) - a.x));
		};
		const long ax0 = -long(v2.y - v1.y), ay0 = long(v2.x - v1.x);
		const long ax1 = -long(v0.y - v2.y), ay1 = long(v0.x - v2.x);
		const long ax2 = -long(v1.y - v0.y), ay2 = long(v1.x - v0.x);

		long e0row = edgeAt(v1, v2, minX, minY);
		long e1row = edgeAt(v2, v0, minX, minY);
		long e2row = edgeAt(v0, v1, minX, minY);

		int written = 0;
		for (int py = minY; py <= maxY; ++py) {
			long e0 = e0row, e1 = e1row, e2 = e2row;
			for (int px = minX; px <= maxX; ++px) {
				bool inside = (e0 >= 0 && e1 >= 0 && e2 >= 0) || (e0 <= 0 && e1 <= 0 && e2 <= 0);
				if (inside) {
					size_t idx = size_t(px) * H + py;
					if (triDepth < depthBuff[idx]) {
						depthBuff[idx] = triDepth;
						++written;
					}
				}
				e0 += ax0; e1 += ax1; e2 += ax2;
			}
			e0row += ay0; e1row += ay1; e2row += ay2;
		}
		return written;
	}

	void depthToBuff()
	{
		float dmin = FAR_DEPTH, dmax = 0.0f;
		for (float d : depthBuff)
			if (d < FAR_DEPTH) { dmin = std::min(dmin, d); dmax = std::max(dmax, d); }

		float range = (dmax > dmin) ? (dmax - dmin) : 1.0f;

		for (uint32_t x = 0; x < W; ++x)
		{
			for (uint32_t y = 0; y < H; ++y)
			{
				float d = depthBuff[size_t(x) * H + y];
				size_t o = size_t(H - 1 - y) * W + x;
				if (d < FAR_DEPTH)
				{
					float t = (d - dmin) / range; 
					unsigned char v = (unsigned char)(255.0f * (1.0f - t)); 
					buff[o] = v;
					colorBuff[o * 3 + 0] = v;
					colorBuff[o * 3 + 1] = v;
					colorBuff[o * 3 + 2] = v;
				}
				else
				{
					buff[o] = 0;
					colorBuff[o * 3 + 0] = 20; 
					colorBuff[o * 3 + 1] = 20;
					colorBuff[o * 3 + 2] = 40;
				}
			}
		}
	}

private:
	//std::unique_ptr<unsigned char[]> buff = nullptr;
	unsigned char* buff = nullptr;
	std::vector<float> depthBuff;
	std::vector<unsigned char> colorBuff; 
	std::unique_ptr<Texture> vizTex;   
	uint32_t W = 0;
	uint32_t H = 0;
};