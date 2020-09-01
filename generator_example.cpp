#include "SCoro.hpp"
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
        T yield_value;

        Initial(T initial_value) noexcept :
            current_value{ initial_value },
            yield_value{ initial_value }
        {}

        bool Poll() noexcept
        {
            yield_value = ++current_value;
            return false;
        }

        T const & Value() const noexcept
        {
            return yield_value;
        }
    };

    using Coro = SCoro::SCoro<Initial>;
    Coro m_coro;
public:
    iota(T const & initial_value) noexcept :
        m_coro{ initial_value }
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

        bool operator==(const iterator& other) const { return false; }
        bool operator!=(const iterator& other) const { return true; }

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
    iota<int> coro(0);

    for (auto v : coro)
        std::cout << v << "\n";

    return 0;
}