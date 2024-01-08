#include <Engine/UnitTests.h>
#include <Core/SIMD.h>
#include <Core/Printer.h>

#include <numbers>
#include <algorithm>


void UnitTests::run()
{
	testSIMD();	
}

void UnitTests::testSIMD()
{
	{
		auto simdedPi = SIMD::convertFloatToSSE(3.1415926);
		//auto desimdedPiArr  = SIMD::convertMM128ToUnalignedFloatArray(simdedPi);
		//for(auto i : desimdedPiArr)
		//	println(i);
		float desimdedPi = SIMD::convertSSEToFloat(simdedPi);
		SIMD::assertEqual(3.1415926,  desimdedPi);
	}

	{
		std::string text = "123456789123456789hhuu789"; 
		char needle[4] = {'h', 'h', 'u', 'u'};

		auto* findRes = SIMD::findFirstSymbolsSse(text.data(), text.data() + text.size(), needle);
		println("result: ", findRes);
	}

	{
		std::array<int, 16> arr = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
		auto sum = SIMD::sumOneBuff(arr.data(), 16);
		//println("sum is ", sum);
		SIMD::assertEqual(sum, 16);
	}

	{
		auto simdedPi = SIMD::convertFloatToSSE(3.1415926);
		auto simdedE = SIMD::convertFloatToSSE(2.71828);
		auto sum = SIMD::sumSSE(simdedPi, simdedE);

		float desimdedSum = SIMD::convertSSEToFloat(sum);
		SIMD::assertEqual(5.8598726, desimdedSum);
	}

	{
		auto val = SIMD::convertFloatToSSE(1,2,3,4);

		float desimdedValX = SIMD::floatGetX(val);
		float desimdedValY = SIMD::floatGetY(val);
		float desimdedValZ = SIMD::floatGetZ(val);
		float desimdedValW = SIMD::floatGetW(val);

		SIMD::assertEqual(1, desimdedValX);
		SIMD::assertEqual(2, desimdedValY);
		SIMD::assertEqual(3, desimdedValZ);
		SIMD::assertEqual(4, desimdedValW);
	}

	{
		auto val = SIMD::convertFloatToSSE(1,2,3,4);	
		auto val2 = SIMD::convertFloatToSSE(1,2,3,4);	

		auto sumDot = SIMD::dot(val, val2);

		//println("dot: ", sumDot);
		//SIMD::assertEqual(20, sumDot);
	}

	{
		std::array<float, 8> buff1;
		std::generate(buff1.begin(), buff1.end(), [](){ return rand(); });

		std::array<float, 8> buff2;
		std::generate(buff2.begin(), buff2.end(), [](){ return rand(); });

		std::array<float, 8> buffSumNotSimd;	
		for(int i = 0; i < 8; ++i){
			buffSumNotSimd[i] = buff1[i] + buff2[i];
		}

		std::array<SIMD::simdSSEFloat, 8> buffSumSimd;	
		SIMD::sumBuffs(buff1.data(), buff2.data(), buffSumSimd.data(), 8);

		for(int i = 0; i < 8; ++i){
			//println(SIMD::convertSSEToFloat(buffSumSimd[i]), " ", buffSumNotSimd[i]);
		}
	}

}
