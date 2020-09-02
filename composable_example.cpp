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

template <unsigned ms>
struct Delay
{
    template <typename T>
    struct type : T
    {
        using T::T;
    private:
        template <typename B>
        struct RecordTime : B
        {
            using B::B;

            time_point start_time;
            bool Poll() noexcept
            {
                start_time = now();
                return true;
            }
        };

        template <typename B>
        struct WaitForExpiry : B
        {
            using B::B;

            bool Poll() noexcept
            {
                return (now() - B::start_time) > std::chrono::milliseconds{ms};
            }
        };

        using states = SCoro::SCoro<RecordTime, WaitForExpiry>;
        states nested_state;
    public:
        bool Poll() noexcept
        {
            return !nested_state.Poll();
        }
    };
};

template <typename T>
struct PrintPollAttempts : T
{
    using T::T;
private:
    template <typename B>
    struct PrintPlus : B
    {
        static bool Poll() noexcept
        {
            std::putchar('/');
            return true;
        }
    };

    template <typename B>
    struct PrintNewline : B
    {
        static bool Poll() noexcept
        {
            std::putchar('\\');
            return true;
        }
    };

    using states = SCoro::SCoro<PrintPlus, PrintNewline>;
    states nested_state;
public:
    bool Poll() noexcept
    {
        std::putchar('\n');
        return !nested_state.Poll();
    }
};

template <typename T>
struct PrintStart : T
{
    const char * input;
    PrintStart(const char * str) : input{str}{}

    bool Poll() noexcept
    {
        std::puts(input);
        return true;
    }
};

template <typename T>
struct PrintTag : T
{
    using T::T;

    bool Poll() noexcept
    {
        std::putchar('\n');
        std::putchar('~');
        std::puts(T::input);
        return true;
    }
};

using Coro = SCoro::SCoro
<
    PrintStart,
    Delay<100>::type,
    PrintPollAttempts,
    Delay<100>::type,
    PrintTag
>;

int main()
{
    Coro coroutine{"composable"};
    while (true)
    {
        while ( coroutine.Poll() )
        {
            std::putchar('-');
            std::this_thread::sleep_for(std::chrono::milliseconds{10});
        }
        // This resets coroutine state.
        coroutine.Reset("composable");
        std::printf("\n\n\n\n");
    }
    return 0;
}