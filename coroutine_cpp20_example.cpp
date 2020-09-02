#include "SCoro.hpp"
#include <experimental/coroutine>
#include <iostream>
#include <Windows.h>

using default_handle = std::experimental::coroutine_handle<>;

template <typename B>
struct Start : B
{
    using B::B;
    static SCoro::Result Poll() noexcept
    {
        std::puts("Waiting for event.");
        return (0 != GetKeyState(0x41));
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
    using promise_handle = std::experimental::coroutine_handle<promise_type>;

    struct promise_type
    {
        SCoroTask get_return_object() noexcept{return { promise_handle::from_promise(*this) };}
        void yield_value(std::nullptr_t){}

        int v = 0;
        void return_value(int value) noexcept { v = value; }
        void unhandled_exception() noexcept {}
        auto initial_suspend() noexcept -> std::experimental::suspend_never{return {};}
        auto final_suspend() noexcept -> std::experimental::suspend_always
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

int main()
{
    auto scoro_runner = SCoroRunner();
    while(!scoro_runner.handle.done())
    {  
        scoro_runner.handle.resume();
        Sleep(100);
    }
    return 0;
}