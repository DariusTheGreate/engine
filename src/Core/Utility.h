#pragma once
#include <utility>
#include <type_traits>

#include <future>
#include <thread>
#include <algorithm>
#include <numeric>
#include <functional>

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