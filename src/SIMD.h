#pragma once 

//TODO(darius) theres troubles here with different versions so work on that
#include <xmmintrin.h>

class SIMD
{
public:
	using simdFloat = __m128;//SSE

	static float convertMM128ToAlignedFloat(simdFloat v)
	{
		alignas(16) float result [4];
		_mm_store_ps (result, v);
	}

	static simdFloat floatSqrt(simdFloat a)
	{
		return (_mm_sqrt_ps(a));
	}

	static float floatGetX(simdFloat v)
	{
		return (_mm_cvtss_f32(v));
	}

	static float floatGetY(simdFloat v)
	{
		auto r = _mm_shuffle_ps(v, v, _MM_SHUFFLE(0, 0, 0, 1));
		return (_mm_cvtss_f32(r));
	}

	static float floadGetZ(simdFloat v)
	{
		auto r = _mm_shuffle_ps(v, v, _MM_SHUFFLE(0, 0, 0, 2));
		return (_mm_cvtss_f32(r));
	}

	static float floadGetW(simdFloat v)
	{
		auto r = _mm_shuffle_ps(v, v, _MM_SHUFFLE(0, 0, 0, 3));
		return (_mm_cvtss_f32(r));
	}

	static float dot(simdFloat a, simdFloat b){
	    simdFloat temp = _mm_mul_ps(a, b);
	    simdFloat temp2 = _mm_shuffle_ps(temp, temp, 0xFE);
	    temp2 = _mm_add_ps(temp, temp2);
	    return _mm_cvtss_f32(_mm_add_ps(temp2, _mm_shuffle_ps(temp, temp, 0xFD)));
	}
};
