#pragma once
#include <type_traits>
#include <utility>

namespace SCoro
{
struct Nothing{};

template <size_t I>
struct Index{};

template <template <typename> class ... Args>
struct Stages;

template <template <typename> class ... Args>
struct ArgList{};

template <size_t count, template <typename> class ...>
struct StagesImpl;

template <size_t count, template <typename> class Arg>
struct StagesImpl<count, Arg> : Arg<Nothing>
{
private:
    using base = Arg<Nothing>;
    static constexpr size_t index = 0;
public:
    StagesImpl & get_at(Index<index>) noexcept { return *this; }
    StagesImpl const & get_at(Index<index>) const noexcept { return *this; }
};

template <size_t count, template <typename> class Arg, template <typename> class ... Args>
struct StagesImpl<count, Arg, Args...> : Arg<StagesImpl<count, Args...>>
{
private:
    using base = Arg<StagesImpl<count, Args...>>;
    static constexpr size_t index = sizeof...(Args);
public:
    using base::get_at;
    StagesImpl & get_at(Index<index>) noexcept { return *this; }
    StagesImpl const & get_at(Index<index>) const noexcept { return *this; }
};

template <typename Base, typename Input, typename Output = ArgList<>>
struct ReverseStages;
template <typename Base, template <typename> class ... Args>
struct ReverseStages<Base, ArgList<>, ArgList<Args...>>
{
    using type = StagesImpl<sizeof...(Args), Args...>;
};
template <typename Base, template <typename> class T, template <typename> class ... Ts, template <typename> class  ... Args>
struct ReverseStages<Base, ArgList<T, Ts...>, ArgList<Args...>>
{
    using type = typename ReverseStages<Base, ArgList<Ts...>, ArgList<T, Args...>>::type;
};

template <size_t index, typename Stgs>
static auto run_fn(Stgs & self) noexcept
{
    return self.get_at(Index<index>{}).Run();
}

template <typename Stgs, size_t ... I>
auto get_impl(Stgs & stack, std::index_sequence<I...>) noexcept
{
    using common_fn_t = std::common_type_t<decltype(run_fn<I, Stgs>)...>;
    common_fn_t fn = nullptr;
    (((I == stack.index) and (fn = run_fn<I, Stgs>)) or ...);
    return fn;
}

template <typename Stgs>
auto get(Stgs & stack) noexcept
{
    return get_impl(stack, std::make_index_sequence<Stgs::count>{});
}

template <template <typename> class ... Args>
struct Stages : ReverseStages<Stages<Args...>, ArgList<Args...>>::type
{
    using base = typename ReverseStages<Stages<Args...>, ArgList<Args...>>::type;
    using base::base;
    using base::get_at;
    static constexpr size_t count = sizeof...(Args);

    mutable size_t index = 0;
    void Reset() noexcept
    {
        std::destroy_at(this);
        new (this) Stages{};
    }
    bool Poll() noexcept
    {
        if (get(*this)(*this)) ++index;
        return index < count;
    }
};
}
