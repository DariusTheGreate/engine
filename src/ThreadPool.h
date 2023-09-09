#include "BlockingQueue.h"
#include <functional>
#include <iostream>
#include <type_traits>
#include <future>

class ThreadPool{
public:
	ThreadPool()
	{
		initialize_workers(3);
	}

	ThreadPool(const size_t& amount){
		initialize_workers(amount);
	}

    ~ThreadPool(){
        this-> Join(); 
    }
   
    template<typename Func>
    void Submit(Func&& f){
        queue.Push([f](){
            f(); 
        });
    }

    template<typename Func, typename... Args>
	auto Submit(Func&& f, Args&&... args){
        using return_type = typename std::invoke_result<Func(Args...)>::type;
        auto task_ptr = std::make_shared<std::packaged_task<return_type()>>([f, args...] () {
                    return f(args...);
                });// not sure about that, its pointer so we not lose it
        auto future_of_task = task_ptr -> get_future();
        auto func_to_add = [task_ptr]() {
            (*task_ptr)();
        };
		queue.Push(func_to_add);
        return future_of_task;
	}

	void Join(){
		for(auto& worker: workers){
			worker.join();
		}
	}

private:
	void initialize_workers(const size_t& amount){
		for(size_t i = 0; i < amount; ++i){
			workers.emplace_back([this]{
				get_task();
			});
		}
	}

	void get_task(){
		while(true){
			auto task = queue.Take();
			if(!task){ 
				break; 
			}
			task();// add exceptions handlers?
		}
	}

    size_t get_threads_count() const {
        return workers.size(); 
    }

    size_t get_task_count() const {
        return queue.size(); 
    }
    
private:
	BlockingQueue<std::function<void()>> queue;
	std::vector<std::thread> workers;
};
