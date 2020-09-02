#include "SCoro.hpp"
#include <thread>
#include <iostream>

template <typename T>
void slow_poll(T & obj)
{
    while(obj.Poll());
}

template <typename T>
struct PrintThreadId : T
{
    SCoro::Result Poll() const noexcept
    {
        std::cout << "thread id = " << std::this_thread::get_id() << "\n";
        return true;
    }
};

template <typename T>
struct SwapThread : T
{
    SCoro::Result Poll() noexcept
    {
        std::thread resumer
        {
            [&]()
            {
                Self().Next();
                slow_poll(Self());
            }
        };
        resumer.detach();
        return SCoro::Result::End;
    }
};

int main()
{
    using scoro = SCoro::SCoro<PrintThreadId, SwapThread, PrintThreadId, SwapThread, PrintThreadId, SwapThread, PrintThreadId>;
    scoro coro;
    slow_poll(coro);

    while(!coro.Done());
    return 0;
}