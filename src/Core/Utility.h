#pragma once
#include <utility>
#include <type_traits>

#include <future>
#include <thread>
#include <algorithm>
#include <numeric>
#include <functional>

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/quaternion.hpp >
#include <glm/gtx/euler_angles.hpp>

template<typename T>
T vmin(T&&t)
{
  return std::forward<T>(t);
}

template<typename T0, typename T1, typename... Ts>
typename std::common_type<
  T0, T1, Ts...
>::type vmin(T0&& val1, T1&& val2, Ts&&... vs)
{
  if (val2 < val1)
    return vmin(val2, std::forward<Ts>(vs)...);
  else
    return vmin(val1, std::forward<Ts>(vs)...);
}


template<typename T>
T vmax(T&&t)
{
  return std::forward<T>(t);
}

template<typename T0, typename T1, typename... Ts>
typename std::common_type<
  T0, T1, Ts...
>::type vmax(T0&& val1, T1&& val2, Ts&&... vs)
{
  if (val2 < val1)
    return vmax(val2, std::forward<Ts>(vs)...);
  else
    return vmax(val1, std::forward<Ts>(vs)...);
}


namespace Parallel
{
	template<typename T>
    unsigned determine_threads(T length) 
    {
        const unsigned long min_per_thread = 3;
        unsigned long max_threads = length / min_per_thread;
        unsigned long hardware_conc = std::thread::hardware_concurrency();
        return vmin(hardware_conc != 0 ? hardware_conc : 2, max_threads);
    }


    template <typename Iterator, typename T>
    T parallel_accumulate(Iterator first, Iterator last, T init)
    {
        using namespace std;
        long length = distance(first, last);

        const unsigned nthreads = determine_threads(length);
        long bsize = length / nthreads;

        vector<future<T>> results(nthreads);

        auto accumulate_block = [](Iterator first, Iterator last) {
         return accumulate(first, last, T{});
        };

        unsigned tidx = 0;
        for (; length >= bsize * (tidx + 1);
            first += bsize, tidx += 1) 
        {
            packaged_task<T(Iterator, Iterator)> task{accumulate_block};
            results[tidx] = task.get_future();
            thread t {move(task), first, first + bsize};
            t.detach();
        }

        auto remainder = length - bsize * tidx;
        T result = init;
        if (remainder > 0) {
            assert(tidx < nthreads);
            result += accumulate_block(first, first + remainder);
        }

        for (unsigned long i = 0; i < nthreads; ++i)
            result += results[i].get();

        return result;
    }


    //todo finish
    template <typename Iterator, typename T>
    void parallel_sort(Iterator first, Iterator last)
    {
        using namespace std;
        long length = distance(first, last);

        const unsigned nthreads = determine_threads(length);
        if(nthreads == 0)
            return;
        long bsize = length / nthreads;

        auto sort_block = [](Iterator first, Iterator last) {
            sort(first, last, std::less{});
            std::cout << "s:";
            for (Iterator i = first; i < last; ++i)
                std::cout << *i;
            std::cout << "\n";
        };
    }

};

namespace Geometry
{
    template<size_t>
	void Slerp(glm::quat q1, glm::quat q2, glm::quat& qr, double lambda) 
	{
		float dotproduct = q1.x * q2.x + q1.y * q2.y + q1.z * q2.z + q1.w * q2.w;
		float theta, st, sut, sout, coeff1, coeff2;

		// algorithm adapted from Shoemake's paper
		lambda=lambda/2.0;

		theta = (float) acos(dotproduct);
		if (theta<0.0) theta=-theta;
		
		st = (float) sin(theta);
		sut = (float) sin(lambda*theta);
		sout = (float) sin((1-lambda)*theta);
		coeff1 = sout/st;
		coeff2 = sut/st;

		qr.x = coeff1*q1.x + coeff2*q2.x;
		qr.y = coeff1*q1.y + coeff2*q2.y;
		qr.z = coeff1*q1.z + coeff2*q2.z;
		qr.w = coeff1*q1.w + coeff2*q2.w;

		glm::normalize(qr);
	}
};