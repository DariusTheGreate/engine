#pragma once

#include <mutex>
#include <vector>
#include <thread>
#include <condition_variable>
#include <iostream>
#include <atomic>

template <typename T> 
class BlockingQueue
{
private:
    std::vector<T> buffer;
    std::mutex buffer_mutex;
    std::condition_variable buffer_size_condition;
    std::atomic<size_t> known_count;
public:
    void Push(T value){
        known_count++;
        {
            std::unique_lock<std::mutex> lock(buffer_mutex);
            buffer.push_back(std::move(value));
        }

        buffer_size_condition.notify_one();
    }

    T Take(){
        bool remove = true;
        std::unique_lock<std::mutex> lock(buffer_mutex);
        while(buffer.empty()){
            buffer_size_condition.wait(lock);
        }

        T result = std::move(buffer.back());//TODO(darius) i dont need move its prval already, right?
        
        if(remove){
            buffer.pop_back();
            known_count--;
        }

        return result;
    }

    void TakeAt(T& spc, int i = 0)
    {
        std::unique_lock<std::mutex> lock(buffer_mutex);

        spc = buffer[i];
    }

    size_t size() const {
        return known_count; 
    }
};
