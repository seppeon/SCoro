#include "scoro/SCoro.hpp"
#include <chrono>
#include <coroutine>
#include <iostream>
#include <random>
#include <string>
#include <thread>

namespace {
    using default_handle = std::coroutine_handle<>;

    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist(1,10);

    template <typename B>
    struct Start : B
    {
        using B::B;
        static SCoro::Result Poll() noexcept
        {
            std::puts("Waiting for event.");
            // RNG serving up a particular number is the async event we are waiting on
            return (dist(rng) == 7);
        }
    };
    template <typename B>
    struct Event : B
    {
        using B::B;
        static SCoro::Result Poll() noexcept
        {
            std::puts("Found event!");
            return true;
        }
    };
    template <typename B>
    struct SubmitValue : B
    {
        using B::B;
        static SCoro::Result Poll() noexcept
        {
            return true;
        }
        int Value() const noexcept
        {
            return 123;
        }
    };

    using scoro = SCoro::SCoro<Start, Event, SubmitValue>;

    struct SCoroTask
    {
        struct promise_type;
        using promise_handle = std::coroutine_handle<promise_type>;

        struct promise_type
        {
            SCoroTask get_return_object() noexcept{return { promise_handle::from_promise(*this) };}
            SCoroTask yield_value(std::nullptr_t){return { promise_handle::from_promise(*this) };}

            int v = 0;
            void return_value(int value) noexcept { v = value; }
            void unhandled_exception() noexcept {}
            auto initial_suspend() noexcept -> std::suspend_never{return {};}
            auto final_suspend() noexcept -> std::suspend_always
            {
                return {};
            }
        };

        bool await_ready() { return true; }
        void await_suspend(default_handle) {}
        int await_resume()
        {
            return handle.promise().v;
        }

        promise_handle handle;
    };

    SCoroTask SCoroRunner()
    {
        scoro manager;
        while(manager.Poll()) co_yield nullptr;
        co_return manager.Value();
    }
}

int main()
{
    auto scoro_runner = SCoroRunner();
    while(!scoro_runner.handle.done())
    {
        scoro_runner.handle.resume();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    return 0;
}
