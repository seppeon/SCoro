#include "SCoro.hpp"
#include <thread>
#include <chrono>
#include <optional>
#include <iostream>

template <typename T>
struct PrintThreadId : T
{
    bool Poll() const noexcept
    {
        std::cout << "thread id = " << std::this_thread::get_id() << "\n";
        return true;
    }
};

template <typename T>
struct ResumeOnNewThread : T
{
    bool Poll() noexcept
    {
        std::thread 
        (
            [&]()
            {
                Self().Inc();
                if (!Self().Done()) Self().Poll();
            }
        ).join();
        return false;
    }
};


int main()
{
    using scoro = SCoro::SCoro<PrintThreadId, ResumeOnNewThread, PrintThreadId, ResumeOnNewThread, PrintThreadId>;

    scoro coro;
    coro.Poll();
    coro.Poll();
    return 0;
}