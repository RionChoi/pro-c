#ifndef MINI_EXEC_HPP
#define MINI_EXEC_HPP

#include <condition_variable>
#include <cstddef>
#include <deque>
#include <functional>
#include <future>
#include <mutex>
#include <thread>
#include <type_traits>
#include <utility>
#include <vector>

namespace mini_exec {

struct Unit {};

template <typename F>
class Sender {
public:
    using value_type = std::invoke_result_t<F&>;

    explicit Sender(F fn)
        : fn_(std::move(fn))
    {
    }

    value_type run()
    {
        return fn_();
    }

private:
    F fn_;
};

template <typename F>
auto make_sender(F fn)
{
    return Sender<F>(std::move(fn));
}

inline auto just()
{
    return make_sender([] { return Unit{}; });
}

template <typename T>
auto just(T value)
{
    return make_sender([value = std::move(value)]() mutable { return value; });
}

template <typename Upstream, typename F>
auto then(Upstream upstream, F fn)
{
    return make_sender([upstream = std::move(upstream), fn = std::move(fn)]() mutable {
        if constexpr (std::is_same_v<typename Upstream::value_type, Unit>) {
            return std::invoke(fn);
        } else {
            return std::invoke(fn, upstream.run());
        }
    });
}

template <typename F>
struct ThenAdaptor {
    F fn;
};

template <typename F>
auto then(F fn)
{
    return ThenAdaptor<F>{std::move(fn)};
}

template <typename Upstream, typename F>
auto operator|(Upstream upstream, ThenAdaptor<F> adaptor)
{
    return then(std::move(upstream), std::move(adaptor.fn));
}

class ThreadPool {
public:
    explicit ThreadPool(std::size_t workerCount)
    {
        if (workerCount == 0U) {
            workerCount = 1U;
        }

        workers_.reserve(workerCount);
        for (std::size_t index = 0; index < workerCount; ++index) {
            workers_.emplace_back([this] { workerLoop(); });
        }
    }

    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;

    ~ThreadPool()
    {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            stopping_ = true;
        }
        cv_.notify_all();

        for (std::thread& worker : workers_) {
            if (worker.joinable()) {
                worker.join();
            }
        }
    }

    template <typename F>
    auto submit(F fn) -> std::future<std::invoke_result_t<F&>>
    {
        using Result = std::invoke_result_t<F&>;
        using Task = std::packaged_task<Result()>;

        auto task = std::make_shared<Task>(std::move(fn));
        std::future<Result> future = task->get_future();

        {
            std::lock_guard<std::mutex> lock(mutex_);
            // ponytail: single shared queue is enough here; work stealing only matters under real contention.
            tasks_.push_back([task] { (*task)(); });
        }
        cv_.notify_one();

        return future;
    }

private:
    void workerLoop()
    {
        while (true) {
            std::function<void()> task;

            {
                std::unique_lock<std::mutex> lock(mutex_);
                cv_.wait(lock, [this] { return stopping_ || !tasks_.empty(); });

                if (stopping_ && tasks_.empty()) {
                    return;
                }

                task = std::move(tasks_.front());
                tasks_.pop_front();
            }

            task();
        }
    }

    std::vector<std::thread> workers_;
    std::deque<std::function<void()>> tasks_;
    std::mutex mutex_;
    std::condition_variable cv_;
    bool stopping_ = false;
};

template <typename Upstream>
auto start(ThreadPool& pool, Upstream upstream)
{
    return pool.submit([upstream = std::move(upstream)]() mutable {
        return upstream.run();
    });
}

template <typename Upstream>
auto on(ThreadPool& pool, Upstream upstream)
{
    return make_sender([&pool, upstream = std::move(upstream)]() mutable {
        return start(pool, std::move(upstream)).get();
    });
}

template <typename Upstream>
decltype(auto) sync_wait(Upstream upstream)
{
    return upstream.run();
}

#if defined(__cpp_lib_sender_receiver) && __cpp_lib_sender_receiver >= 202202L
inline constexpr bool nativeSendersAvailable = true;
#else
inline constexpr bool nativeSendersAvailable = false;
#endif

}  // namespace mini_exec

#endif
