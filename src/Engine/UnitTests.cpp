#include <Engine/UnitTests.h>
#include <Core/SIMD.h>
#include <Core/Printer.h>
#include <Core/Utility.h>

#include <numbers>
#include <algorithm>
#include <map>
#include <limits>
#include <iostream>
#include <random>

void UnitTests::run()
{
	testSIMD();	
}

void UnitTests::testSIMD()
{
	{
		auto simdedPi = SIMD::convertFloatToSSE(3.1415926f);
		//auto desimdedPiArr  = SIMD::convertMM128ToUnalignedFloatArray(simdedPi);
		//for(auto i : desimdedPiArr)
		//	println(i);
		float desimdedPi = SIMD::convertSSEToFloat(simdedPi);
		SIMD::assertEqual(3.1415926f,  desimdedPi);
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
		SIMD::assertEqual(static_cast<float>(sum), 16);
	}

	{
		auto simdedPi = SIMD::convertFloatToSSE(3.1415926f);
		auto simdedE = SIMD::convertFloatToSSE(2.71828f);
		auto sum = SIMD::sumSSE(simdedPi, simdedE);

		float desimdedSum = SIMD::convertSSEToFloat(sum);
		SIMD::assertEqual(5.8598726f, desimdedSum);
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
		std::string buff1;
		int i = 0;
		while(i++ < 256){
			buff1.append("q");
		}

		std::string buff2 = "";
		buff2.resize(buff1.size());
		SIMD::memcpyBigChank(buff1.data(), buff2.data(), buff1.size());	

		println(buff2);
		println(buff2.size());
	}

	{
		std::array<float, 8> buff1;
		std::generate(buff1.begin(), buff1.end(), [](){ return static_cast<float>(rand()); });

		std::array<float, 8> buff2;
		std::generate(buff2.begin(), buff2.end(), [](){ return static_cast<float>(rand()); });

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


	{
		glm::vec4 x1 = { 1.0f, 0.0f, 0.0f, 0.0f};
		glm::vec4 y1 = { 0.0f, 1.0f, 0.0f,0.0f };
		glm::vec4 z1 = { 0.0f, 0.0f, 1.0f,0.0f };

		glm::mat4 r1(x1, y1, z1, glm::vec4(0.0f));

		glm::quat q1(r1);

		glm::mat4 r2(y1, x1, z1, glm::vec4(0.0f));

		glm::quat q2(r2);
	}

}
