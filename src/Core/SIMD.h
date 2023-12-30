#pragma once 

//TODO(darius) theres troubles here with different versions so work on that
//NOTE(darius) _mm<size>_<action>_<type>
#include <xmmintrin.h>
#include <array>
#include <cassert>

class SIMD
{
public:

//TODO(darius) UNITTEST THIS SHIT
//TODO(darius) USE atleast AVX WTF


//SSE:
public:
	using simdSSEFloat = __m128;//SSE

	static std::array<float, 4> convertMM128ToUnalignedFloatArray(simdSSEFloat v)
	{
		//alignas(16) float result [4];
		std::array<float, 4> result;
		_mm_storeu_ps (result.data(), v);//NOTE(darius) witout u in case of aligned

		return result;
	}

	static float convertSSEToFloat(simdSSEFloat v)
	{
		return _mm_cvtss_f32(v);
	}

	//DANGER(darius) WILL crash if v not aligned, use _mm_loadu_ps for unaligned
	static simdSSEFloat convertFloatToSSE(float v)
	{
		return _mm_loadu_ps(&v);
	}

	static simdSSEFloat convertFloatToSSE(float x, float y, float z, float w)
	{
		alignas(16) float alignedBuff[4] = {0};

		alignedBuff[0] = x;
		alignedBuff[1] = y;
		alignedBuff[2] = z;
		alignedBuff[3] = w;

		return _mm_loadu_ps((const float*)&alignedBuff);
	}

	static inline simdSSEFloat sumSSE(simdSSEFloat a, simdSSEFloat b)
	{
		return _mm_add_ps(a, b);
	}

	static simdSSEFloat floatSqrt(simdSSEFloat a)
	{
		return (_mm_sqrt_ps(a));
	}

	static float floatGetX(simdSSEFloat v)
	{
		return (_mm_cvtss_f32(v));
	}

	static float floatGetY(simdSSEFloat v)
	{
		auto r = _mm_shuffle_ps(v, v, _MM_SHUFFLE(0, 0, 0, 1));
		return (_mm_cvtss_f32(r));
	}

	static float floatGetZ(simdSSEFloat v)
	{
		auto r = _mm_shuffle_ps(v, v, _MM_SHUFFLE(0, 0, 0, 2));
		return (_mm_cvtss_f32(r));
	}

	static float floatGetW(simdSSEFloat v)
	{
		auto r = _mm_shuffle_ps(v, v, _MM_SHUFFLE(0, 0, 0, 3));
		return (_mm_cvtss_f32(r));
	}

	//NOTE(darius) garbage
	static float dot(simdSSEFloat a, simdSSEFloat b){
	    simdSSEFloat temp = _mm_mul_ps(a, b);
	    simdSSEFloat temp2 = _mm_shuffle_ps(temp, temp, 0xFE);
	    temp = sumSSE(temp, temp2);
	    return _mm_cvtss_f32(sumSSE(temp2, _mm_shuffle_ps(temp, temp, 0xFD)));
	}

	//TODO(darius) add restrict classifier
	//template<typename T>
	static void sumBuffs(float* a, float* b, simdSSEFloat* res, size_t sz)
	{
		simdSSEFloat sseA;
		simdSSEFloat sseB;
		simdSSEFloat sseSum;

		int i = 0;

		//NOTE(darius) garbage
		for(i = 0; i < sz; i += sizeof(float) * 4){
			sseA = convertFloatToSSE(a[i], a[i+1], a[i+2], a[i+3]);
			sseB = convertFloatToSSE(b[i], b[i+1], b[i+2], b[i+3]);
			//sseSum = sumSSE(sseA, sseB);

			res[i] = (sseB);
		}

		if(i - sizeof(float) < sz){
			//finish analigned part..
		}
	}

	static void assertEqual(float a, float b)
	{
		float EPS = 0.00001;
		assert(std::abs(a - b) < EPS);
	}

//TODO(darius) AVX here
public:

};
