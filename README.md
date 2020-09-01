# SCoro

A stackless, resumable, resettable coroutine library for C++17.
This was made when I discovered that C++20 coroutines very frequently require heap allocation and their embodied types are erased. 

This library allows:
- Resettable coroutines
- Resumable coroutines
- Easy scheduling on other threads
- State stored on stack or heap

Upcomming:
- Allow arbitary return types
- Allow arbitary parameters
- Enable exceptions.

An example of how to use the code is here:

```CPP
#include "SCoro.hpp"
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

template <typename T>
struct Initial : T
{
    duration wait_time = std::chrono::seconds{1};

    bool Run() const noexcept
    {
        return true;
    }
};

template <typename T>
struct RecordTime : T
{
    time_point start_time;

    bool Run() noexcept
    {
        start_time = now();
        return true;
    }
};

template <typename T>
struct WaitForExpiry : T
{
    unsigned retries = 0;
    bool Run() noexcept
    {
        ++retries;
        return (now() - T::start_time) > T::wait_time;
    }
};

template <typename T>
struct PrintPollAttempts : T
{
private:
    template <typename B>
    struct PrintPlus : B
    {
        static bool Run() noexcept
        {
            std::printf("/");
            return true;
        }
    };

    template <typename B>
    struct PrintNewline : B
    {
        static bool Run() noexcept
        {
            std::printf("\\");
            return true;
        }
    };

    using stack = Stages<PrintPlus, PrintNewline>;
    stack nested_state;
public:
    bool Run() noexcept
    {
        std::printf("\n");
        return not nested_state.Poll();
    }
};

using Coro = Stages
<
    Initial,
    RecordTime,
    WaitForExpiry,
    PrintPollAttempts,
    RecordTime
>;

int main()
{
    auto coroutine = std::make_shared<Coro>();
    while (true)
    {
        coroutine->Poll();

        std::thread next_thread
        {
            [](std::shared_ptr<Coro> coro)
            {
                while (coro->Poll())
                {
                    std::printf("-");
                    std::this_thread::sleep_for(std::chrono::milliseconds{10});
                }
                std::printf("\n\n");
                coro->Reset();
            },
            coroutine
        };

        next_thread.join();
    }
    return 0;
}
```
