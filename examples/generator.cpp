#include "scoro/SCoro.hpp"
#include <iostream>
#include <optional>

template <typename T>
class iota
{
private:
    template <typename B>
    struct Initial : B
    {
        T current_value;
        T end_value;

        Initial(T const & from, T const & to) noexcept :
            current_value{ from },
            end_value{ to }
        {}

        SCoro::Result Poll() noexcept
        {
            return ++current_value > end_value ? SCoro::Result::Next : SCoro::Result::Yield;
        }

        T const & Value() const noexcept
        {
            return current_value;
        }
    };

    using Coro = SCoro::SCoro<Initial>;
    Coro m_coro;
public:
    iota(T const & from, T const & to) noexcept :
        m_coro{ from, to }
    {}

    struct iterator
    {
        Coro & m_coro;

        typedef T                     value_type;
        typedef std::ptrdiff_t        difference_type;
        typedef T*                    pointer;
        typedef T&                    reference;

        T const & operator*() noexcept
        {
            return m_coro.Value();
        }

        bool operator==(const iterator& other) const { return m_coro.Done(); }
        bool operator!=(const iterator& other) const { return !operator==(other); }

        iterator& operator++()
        {
            m_coro.Poll();
            return *this;
        }
    };

    iterator begin() noexcept
    {
        return {m_coro};
    }
    iterator end() noexcept
    {
        return {m_coro};
    }
};

int main()
{
    iota<int> coro(0, 10);

    for (auto v : coro)
        std::cout << v << "\n";

    return 0;
}