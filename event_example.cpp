#include "SCoro.hpp"
#include <thread>
#include <chrono>
#include <string>
#include <iostream>

void event()
{

}
template <typename B>
struct Start : B
{
    using B::B;
    bool Poll() noexcept
    {
        std::puts("Waiting for event.");
        return true;
    }
};
template <typename B>
struct Event : B
{
    using B::B;
    bool Poll() noexcept
    {
        std::puts("Found event!");
        return true;
    }
};

int main()
{
    auto coro = std::make_unique<SCoro::SCoro<Start, Event>>();
    // simulate async event
    coro->Poll();
    std::thread thread
    {
        [](decltype(coro) continue_after_event)
        {
            std::string input;
            std::getline(std::cin, input);
            if (input.compare("x") == 0)
            {
                continue_after_event->Poll();
            }
        },
        std::move(coro)
    };

    thread.join();
}