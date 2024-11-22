#include <boost/asio/execution.hpp>
#include <condition_variable>
#include <iostream>
#include <memory>
#include <mutex>
#include <queue>

namespace execution = boost::asio::execution;

namespace custom_props {

	struct Priority
	{
		static constexpr bool is_requirable = true;
		static constexpr bool is_preferable = true;

		using polymorphic_query_result_type = int;

		int value() const { return value_; }

		int value_ = 1;
	};

	constexpr Priority low_priority{ 0 };
	constexpr Priority normal_priority{ 1 };
	constexpr Priority high_priority{ 2 };
}


class PriorityScheduler
{
public:
	class Executor
	{
	public:
		Executor(PriorityScheduler& ctx) noexcept
			: context_(ctx), priority_(custom_props::normal_priority.value())
		{
		}

		PriorityScheduler& query(execution::context_t) const noexcept
		{
			return context_;
		}

		int query(custom_props::Priority) const noexcept
		{
			return priority_;
		}

		Executor require(custom_props::Priority pri) const
		{
			Executor new_ex(*this);
			new_ex.priority_ = pri.value();
			return new_ex;
		}

		template <class Func>
		void execute(Func f) const
		{
			auto p(std::make_shared<item<Func>>(priority_, std::move(f)));
			std::lock_guard<std::mutex> lock(context_.locker);
			context_.queue.push(p);
			context_.condition.notify_one();
		}

		friend bool operator==(const Executor& a,
			const Executor& b) noexcept
		{
			return &a.context_ == &b.context_;
		}

		friend bool operator!=(const Executor& a,
			const Executor& b) noexcept
		{
			return &a.context_ != &b.context_;
		}

	private:
		PriorityScheduler& context_;
		int priority_;
	};

	Executor executor() noexcept
	{
		return Executor(*const_cast<PriorityScheduler*>(this));
	}

	void run()
	{
		std::unique_lock<std::mutex> lock(locker);
		for (;;)
		{
			condition.wait(lock, [&] { return stopped || !queue.empty(); });
			if (stopped)
				return;
			auto p(queue.top());
			queue.pop();
			lock.unlock();
			p->execute_(p);
			lock.lock();
		}
	}

	void stop()
	{
		std::lock_guard<std::mutex> lock(locker);
		stopped = true;
		condition.notify_all();
	}

private:
	struct item_base
	{
		int priority_;
		void (*execute_)(std::shared_ptr<item_base>&);
	};

	template <class Func>
	struct item : item_base
	{
		item(int pri, Func f) : function_(std::move(f))
		{
			priority_ = pri;
			execute_ = [](std::shared_ptr<item_base>& p)
			{
				Func tmp(std::move(static_cast<item*>(p.get())->function_));
				p.reset();
				tmp();
			};
		}

		Func function_;
	};

	struct item_comp
	{
		bool operator()(
			const std::shared_ptr<item_base>& a,
			const std::shared_ptr<item_base>& b)
		{
			return a->priority_ < b->priority_;
		}
	};

	std::mutex locker;
	std::condition_variable condition;
	std::priority_queue<
		std::shared_ptr<item_base>,
		std::vector<std::shared_ptr<item_base>>,
		item_comp> queue;
	bool stopped = false;
};
