#pragma once 

//TODO(darius) theres troubles here with different versions so work on that
//NOTE(darius) _mm<size>_<action>_<type>
#include <xmmintrin.h>
#include <emmintrin.h>
#include <intrin.h>

#include <array>
#include <cassert>

class SIMD
{
public:

//TODO(darius) UNITTEST THIS SHIT
//TODO(darius) USE atleast AVX WTF


//SSE:k	
//#if defined(__SSE2__)

public:
	using simdSSEFloat = __m128;
	using simdSSEInt = __m128i;

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


	template <char ...chars>
	static constexpr bool is_in(char x) 
	{ 
		return ((x == chars) || ...); 
	}

	template <char s0	>
	static inline simdSSEInt mm_is_in(simdSSEInt bytes)
	{
	    simdSSEInt eq0 = _mm_cmpeq_epi8(bytes, _mm_set1_epi8(s0));
	    return eq0;
	}

	template <char... symbols>
	static inline const char* findFirstSymbolsSse(const char* const begin, const char* const end)
	{
	    const char* pos = begin;

	    for (; pos + 15 < end; pos += 16)
	    {
	    	//TODO(darius) create 4 masks with pos; pos + 1; pos + 2; pos + 3, etc
	        simdSSEInt bytes = _mm_loadu_si128(reinterpret_cast<const simdSSEInt *>(pos));

	        simdSSEInt eq = mm_is_in<symbols...>(bytes);

	        uint16_t bit_mask = uint16_t(_mm_movemask_epi8(eq));

	        //NOTE(darius) dont use builtin?
	        if (bit_mask)
	            return pos + _tzcnt_u64 (bit_mask);
	    }

	    //NOTE(darius) tail
	    for (; pos < end; ++pos)
	        if (is_in<symbols...>(*pos))
	            return pos;

	    return end;
	}

	static void assertEqual(float a, float b)
	{
		float EPS = 0.00001;
		assert(std::abs(a - b) < EPS, "Assertion of floats failed!");
	}

	static void assertEqual(const char* const a, const char* const b)
	{
		auto str_A = _mm_load_si128((__m128i*)(a)); 
		auto str_B = _mm_load_si128((__m128i*)(b)); 

		auto char_eq = (_mm_cmpeq_epi8(str_A, str_B)); 
		uint16_t bit_mask = uint16_t(_mm_movemask_epi8(char_eq));
		assert(~bit_mask != 0, "Assertion of char* failed!");		
	}

//TODO(darius) AVX here
public:

};
