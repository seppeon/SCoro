#include "scoro/SCoro.hpp"
#include <thread>
#include <chrono>
#include <iostream>

using system_clock = std::chrono::system_clock;
using time_point = std::chrono::time_point<system_clock>;
using duration = typename system_clock::duration;

static time_point now() noexcept
{
    return system_clock::now();
}

template <typename B>
struct Initial : B
{
    duration wait_time = std::chrono::seconds{1};

    SCoro::Result Poll() const noexcept
    {
        return true;
    }
};

template <typename B>
struct RecordTime : B
{
    using B::B;
    time_point start_time;

    SCoro::Result Poll() noexcept
    {
        start_time = now();
        return true;
    }
};

template <typename B>
struct WaitForExpiry : B
{
    using B::B;
    unsigned retries = 0;
    SCoro::Result Poll() noexcept
    {
        ++retries;
        return (now() - B::start_time) > B::wait_time;
    }
};

template <typename T>
struct PrintPollAttempts : T
{
    using T::T;
private:
    template <typename B>
    struct PrintPlus : B
    {
        static SCoro::Result Poll() noexcept
        {
            std::printf("/");
            return true;
        }
    };

    template <typename B>
    struct PrintNewline : B
    {
        static SCoro::Result Poll() noexcept
        {
            std::printf("\\");
            return true;
        }
    };

    using states = SCoro::SCoro<PrintPlus, PrintNewline>;
    states nested_state;
public:
    SCoro::Result Poll() noexcept
    {
        std::printf("\n");
        return !nested_state.Poll();
    }
};

using Coro = SCoro::SCoro
<
    Initial,
    RecordTime,
    WaitForExpiry,
    PrintPollAttempts,
    RecordTime
>;

int main()
{
    auto coroutine = Coro{};
    while (true)
    {
        while (coroutine.Poll())
        {
            std::printf("-");
            std::this_thread::sleep_for(std::chrono::milliseconds{10});
        }
        std::printf("\n\n");
        coroutine.Reset();
    }
    return 0;
}