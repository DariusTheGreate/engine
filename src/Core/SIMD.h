#pragma once 

//TODO(darius) theres troubles here with different versions so work on that
//NOTE(darius) _mm<size>_<action>_<type>
#include <xmmintrin.h>

class SIMD
{
//TODO(darius) UNITTEST THIS SHIT
public:
	using simdSSEFloat = __m128;//SSE

	static float convertMM128ToAlignedFloat(simdSSEFloat v)
	{
		alignas(16) float result [4];
		_mm_store_ps (result, v);
	}

	static simdSSEFloat convertFloatToSSE(float v)
	{
		return _mm_loadu_ps(&v);
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

	static float floadGetZ(simdSSEFloat v)
	{
		auto r = _mm_shuffle_ps(v, v, _MM_SHUFFLE(0, 0, 0, 2));
		return (_mm_cvtss_f32(r));
	}

	static float floadGetW(simdSSEFloat v)
	{
		auto r = _mm_shuffle_ps(v, v, _MM_SHUFFLE(0, 0, 0, 3));
		return (_mm_cvtss_f32(r));
	}

	static float dot(simdSSEFloat a, simdSSEFloat b){
	    simdSSEFloat temp = _mm_mul_ps(a, b);
	    simdSSEFloat temp2 = _mm_shuffle_ps(temp, temp, 0xFE);
	    temp = sumSSE(temp, temp2);
	    return _mm_cvtss_f32(sumSSE(temp2, _mm_shuffle_ps(temp, temp, 0xFD)));
	}

	//template<typename T>
	static void sumBuffs(float* a, float* b, float* res, size_t sz)
	{
		simdSSEFloat sseA;
		simdSSEFloat sseB;
		simdSSEFloat sseSum;

		int i = 0;

		//NOTE(darius) i think it cxan be done without iterations
		for(i = 0; i < sz; i += sizeof(float)){
			sseA = convertFloatToSSE(a[i]);
			sseB = convertFloatToSSE(b[i]);
			sseSum = sumSSE(sseA, sseB);

			res[i] = convertMM128ToAlignedFloat(sseSum);
		}

		if(i - sizeof(float) < sz){
			//finish analigned part..
		}
	}
};
