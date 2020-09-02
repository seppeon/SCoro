#include "SCoro.hpp"
#include <thread>
#include <chrono>
#include <string>
#include <iostream>

struct PauseResume
{
    template <typename B>
    struct Start : B
    {
        using B::B;
        static SCoro::Result Poll() noexcept
        {
            std::puts("Waiting for event.");
            return true;
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
    using scoro = SCoro::SCoro<Start, Event>;
};

int main()
{
    auto coro = std::make_unique<PauseResume::scoro>();
    coro->Poll();
    // simulate async event
    // This could be finished by an interrupt or scheduler, timer, IOCP etc...
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