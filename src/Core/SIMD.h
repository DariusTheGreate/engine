#pragma once 

//TODO(darius) theres troubles here with different versions so work on that
//NOTE(darius) _mm<size>_<action>_<type>
#include <xmmintrin.h>
#include <emmintrin.h>
#include <intrin.h>

#include <array>
#include <cassert>

#include <Core/Printer.h>

class SIMD
{
public:

//TODO(darius) UNITTEST THIS SHIT
//TODO(darius) USE atleast AVX WTF

//https://www.laruence.com/sse/#expand=3333,3333,1890,5019,6151,5022,94,5653,94,3864,4906,4966,3864,4906,4906,789,4070,5944&techs=SSE,SSE2,SSE3,SSSE3,SSE4_1,SSE4_2
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

	static simdSSEInt broadcastSSEInt(int a)
	{
		return _mm_set1_epi32(a);
	}

	static simdSSEInt getZeroSSEInt()
	{
		return _mm_setzero_si128();
	}

    //NOTE(darius) Create mask from the most significant bit of each 8-bit element in input, and store the result in res
	static uint16_t getMask(simdSSEInt a)
	{
		return uint16_t(_mm_movemask_epi8(a));
	}

	static simdSSEInt getXOR(simdSSEInt a, simdSSEInt b)
	{
		return _mm_or_si128(a, b); 
	}

	static int testZero(simdSSEInt a, simdSSEInt b)
	{
		return _mm_testz_si128(a,b);
	}

	//NOTE(darius) just to not forget after many years: (1,1,1,1) + (1,1,1,1) = (2,2,2,2)
	//NOTE(darius) https://www.cs.virginia.edu/~cr4bd/3330/S2018/simdref.html
	static simdSSEInt addPackedSSEInts(simdSSEInt a, simdSSEInt b)
	{
		return _mm_add_epi32(a, b);	
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
		//simdSSEFloat sseSum;

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

	//TODO(darius) wrong size, its not 256, its 128
	static int sumOneBuff(int* a, int n)
	{
		simdSSEInt s = getZeroSSEInt(); 
		simdSSEInt s2 = getZeroSSEInt(); 

		for(int i = 0; i < n; i += 8){
			s = addPackedSSEInts(s, _mm_loadu_si128(reinterpret_cast<const simdSSEInt *>(&a[i])));
			s2 = addPackedSSEInts(s2, _mm_loadu_si128(reinterpret_cast<const simdSSEInt *>(&a[i+4])));
		}

		s = addPackedSSEInts(s,s2);

		int t[4];
		int horizontalS = 0;
		_mm_storeu_si128((simdSSEInt*)t, s);

		for(int i = 0; i < 4; ++i){
			horizontalS += t[i];
		}

		return horizontalS;
	}

	static simdSSEInt mm_is_in(simdSSEInt bytes, simdSSEInt needle)
	{
	    return _mm_cmpeq_epi8(bytes, needle);
	}

	static const char* findFirstSymbolsSse(char* begin, char* end, char needle[4])
	{
		auto vectorisedNeedle = _mm_loadu_si128((simdSSEInt*)&needle[0]);// _mm_set_epi32((int)needle);
		//println("vetoised needle: ", (char*)&vectorisedNeedle);

	    char* pos = begin;

	    for (; pos + 7 < end; pos += 8)
	    {
	        simdSSEInt bytes = _mm_loadu_si128(reinterpret_cast<const simdSSEInt *>(pos));
	        simdSSEInt eq = mm_is_in(bytes, vectorisedNeedle);

	        simdSSEInt bytes2 = _mm_loadu_si128(reinterpret_cast<const simdSSEInt *>(pos+1));
	        simdSSEInt eq2 = mm_is_in(bytes2, vectorisedNeedle);

	        simdSSEInt bytes3 = _mm_loadu_si128(reinterpret_cast<const simdSSEInt *>(pos+2));
	        simdSSEInt eq3 = mm_is_in(bytes3, vectorisedNeedle);

	        simdSSEInt bytes4 = _mm_loadu_si128(reinterpret_cast<const simdSSEInt *>(pos+3));
	        simdSSEInt eq4 = mm_is_in(bytes4, vectorisedNeedle);

	        uint16_t bit_mask = getMask(eq);
	        uint16_t bit_mask2 = getMask(eq2);
	        uint16_t bit_mask3 = getMask(eq3);
	        uint16_t bit_mask4 = getMask(eq4);

	        //println("bit_mask: ", bit_mask);
	        //println("bit_mask2: ", bit_mask2);
	        //println("bit_mask3: ", bit_mask3);
	        //println("bit_mask4: ", bit_mask4);

	        /*simdSSEInt m12 = getXOR(eq, eq2);
	        simdSSEInt m34 = getXOR(eq3, eq4);
	        simdSSEInt m = getXOR(m12, m34);
	        if (!testZero(m, m)) {
	            unsigned mask = (getMask(eq4) << 24)
	                          + (getMask(eq3) << 16)
	                          + (getMask(eq2) << 8)
	                          +  getMask(eq);
	            return pos + _tzcnt_u64(mask);
	        }*/

	        uint16_t res_mask = bit_mask | bit_mask2 | bit_mask3 | bit_mask4;

	        //NOTE(darius) dont use builtin?
	        if (res_mask){
	        	println("!resMask positive! ", pos);
	        	for(int i = 0; i < 4; ++i){
	        		if(assertEqual(pos + i, &needle[0], 4))
	        			return pos + i;
	        	}
	            //return pos + _tzcnt_u64 (res_mask);
	        }
	    }

	    //NOTE(darius) tail
	    /*for (; pos < end; ++pos){
	    	if(std::strcmp(pos, needle) == 0);
	    		return pos;
	    }*/

	    return end;
	}

	static void assertEqual(float a, float b)
	{
	    float EPS = 0.00001f;
		if (!(std::abs(a - b) < EPS)) {
			std::cout << "Assertion of floats failed!" << std::endl;
			assert(std::abs(a - b) < EPS);
		}
	}

	static bool assertEqual(char* a, char* b, int len)
	{
		//NOTE(darius) only 128 bits
		/*auto str_A = _mm_load_si128((__m128i*)(a)); 
		auto str_B = _mm_load_si128((__m128i*)(b)); 

		auto char_eq = (_mm_cmpeq_epi8(str_A, str_B)); 
		uint16_t bit_mask = uint16_t(_mm_movemask_epi8(char_eq));
		assert(~bit_mask != 0, "Assertion of char* failed!");		
		*/

		//for(; a != aend && *a == *b; a++, b++)
		//	;
		//return a == aend;

		for(int i = 0; i < len; ++i){
			if(a[i] != b[i])
				return false;
		}

		return true;
	}

	static void* memcpyBigChank(void* src_, void* dst_, size_t size)
	{
		char * __restrict dst = reinterpret_cast<char * __restrict>(dst_);
	    const char * __restrict src = reinterpret_cast<const char * __restrict>(src_);
	    size_t padding = (16 - (reinterpret_cast<size_t>(dst) & 15)) & 15;

        if (padding > 0)
        {
            __m128i head = _mm_loadu_si128(reinterpret_cast<const __m128i*>(src));
            _mm_storeu_si128(reinterpret_cast<__m128i*>(dst), head);
            dst += padding;
            src += padding;
            size -= padding;
        }

        /// Aligned unrolled copy. We will use half of available SSE registers.
        /// It's not possible to have both src and dst aligned.
        /// So, we will use aligned stores and unaligned loads.
        __m128i c0, c1, c2, c3, c4, c5, c6, c7;

        while (size >= 128)
        {
            c0 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(src) + 0);
            c1 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(src) + 1);
            c2 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(src) + 2);
            c3 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(src) + 3);
            c4 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(src) + 4);
            c5 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(src) + 5);
            c6 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(src) + 6);
            c7 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(src) + 7);
            src += 128;
            _mm_store_si128((reinterpret_cast<__m128i*>(dst) + 0), c0);
            _mm_store_si128((reinterpret_cast<__m128i*>(dst) + 1), c1);
            _mm_store_si128((reinterpret_cast<__m128i*>(dst) + 2), c2);
            _mm_store_si128((reinterpret_cast<__m128i*>(dst) + 3), c3);
            _mm_store_si128((reinterpret_cast<__m128i*>(dst) + 4), c4);
            _mm_store_si128((reinterpret_cast<__m128i*>(dst) + 5), c5);
            _mm_store_si128((reinterpret_cast<__m128i*>(dst) + 6), c6);
            _mm_store_si128((reinterpret_cast<__m128i*>(dst) + 7), c7);
            dst += 128;

            size -= 128;
        }

        return src_;
	}

//TODO(darius) AVX here
public:

};
