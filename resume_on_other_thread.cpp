#include "SCoro.hpp"
#include <thread>
#include <chrono>
#include <optional>
#include <mutex>
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


int main()
{
    using scoro = SCoro::SCoro<PrintThreadId, PrintThreadId>;
    scoro coro;
    coro.Poll();
    std::thread resumer
    {
        [&]()
        {
            while(coro.Poll());
        }
    };
    resumer.join();
    return 0;
}