#pragma once
#include <type_traits>
#include <utility>
#include <memory>

namespace SCoro
{
    struct Result
    {
        enum
        {
            End,
            Yield,
            Next,
        }
        value;
        constexpr Result(decltype(value) input) noexcept :
            value{ input }
        {}
        constexpr Result(bool input) noexcept :
            value{ input ? Next : Yield }
        {}

        constexpr bool operator == (decltype(value) rhs) const noexcept
        {
            return value == rhs;
        }
        constexpr bool operator !=(decltype(value) rhs) const noexcept
        {
            return !operator==(rhs);
        }

        constexpr operator bool() const noexcept
        {
            return value != End;
        }
    };

    namespace Impl
    {
        struct Nothing{};

        template <size_t I>
        struct Index{};

        template <template <typename> class ... Args>
        struct Stages;

        template <template <typename> class ... Args>
        struct ArgList{};

        template <typename Base, size_t count, template <typename> class ...>
        struct StagesImpl;

        template <typename Base, size_t count, template <typename> class Arg>
        struct StagesImpl<Base, count, Arg> : Arg<Nothing>
        {
            using base = Arg<Nothing>;
            using base::base;
        private:
            static constexpr size_t index = 0;
        public:
            constexpr Base const & Self() const noexcept { return static_cast<Base&>(*this); }
            constexpr Base & Self() noexcept { return static_cast<Base&>(*this); }

            constexpr StagesImpl & get_at(Index<index>) noexcept { return *this; }
            constexpr StagesImpl const & get_at(Index<index>) const noexcept { return *this; }
        };

        template <typename Base, size_t count, template <typename> class Arg, template <typename> class ... Args>
        struct StagesImpl<Base, count, Arg, Args...> : Arg<StagesImpl<Base, count, Args...>>
        {
            using base = Arg<StagesImpl<Base, count, Args...>>;
            using base::base;
        private:
            static constexpr size_t index = sizeof...(Args);
        public:
            using base::Self;
            using base::get_at;
            constexpr StagesImpl & get_at(Index<index>) noexcept { return *this; }
            constexpr StagesImpl const & get_at(Index<index>) const noexcept { return *this; }
        };

        template <typename Base, typename Input, typename Output = ArgList<>>
        struct ReverseStages;

        template <typename Base, template <typename> class ... Args>
        struct ReverseStages<Base, ArgList<>, ArgList<Args...>>
        {
            using type = StagesImpl<Base, sizeof...(Args), Args...>;
        };

        template <typename Base, template <typename> class T, template <typename> class ... Ts, template <typename> class  ... Args>
        struct ReverseStages<Base, ArgList<T, Ts...>, ArgList<Args...>>
        {
            using type = typename ReverseStages<Base, ArgList<Ts...>, ArgList<T, Args...>>::type;
        };

        template <size_t index, typename Stgs>
        constexpr static Result poll_fn(Stgs & self) noexcept
        {
            return self.get_at(Index<index>{}).Poll();
        }

        template <typename Stgs, typename>
        struct GetImpl;
        template <typename Stgs, size_t ... I>
        struct GetImpl<Stgs, std::index_sequence<I...>>
        {
            using common_fn_t = std::common_type_t<decltype(poll_fn<I, Stgs>)...>;
            static constexpr common_fn_t lut[sizeof...(I)]{ poll_fn<I, Stgs>... };
            static constexpr auto get(Stgs & stack) noexcept
            {
                return lut[stack.Index()];
            }
        };

        template <typename Stgs>
        constexpr auto get(Stgs & stack) noexcept
        {
            return GetImpl<Stgs, std::make_index_sequence<Stgs::count>>::get(stack);
        }
    }

    template <size_t count>
    struct EboIndex
    {
        mutable size_t index = 0;
        constexpr size_t Index() const noexcept { return index; }
        constexpr void Inc() const noexcept { ++index; }
    };

    template <>
    struct EboIndex<1>
    {
        constexpr size_t Index() const noexcept { return 0; }
        constexpr void Inc() const noexcept {}
    };

    template <template <typename> class ... Args>
    struct Checker
    {
        static constexpr bool all_have_base = (std::is_base_of_v<Impl::Nothing, Args<Impl::Nothing>> && ...);
        static_assert( all_have_base, R"(All classes passed into SCoro must have a single template parameter which is inherited from: 'template <typename B> struct Foo : B { using B::B; ... };')");
    };


    template <typename Obj>
    constexpr auto Poll(Obj & obj) noexcept
    {
        switch (Impl::get(obj)(obj).value)
        {
        case Result::Next:
            obj.Inc();
            return obj.Index() < obj.count ? Result::Next : Result::End;
        case Result::Yield:
            return Result::Yield;    
        default:
            return Result::End;
        }
    } 

    template <typename B, size_t offset>
    struct Nexter : B
    {
        using B::B;
        using B::Done;
        using B::Poll;
        using B::count;

        constexpr size_t Index() const noexcept { return B::Index() + offset; }
        constexpr void Inc() const noexcept { return B::Inc(); }
        
        constexpr auto & Next() noexcept
        {
            return static_cast<Nexter<B, offset + 1> &>(*this);
        }
        constexpr auto Poll() noexcept
        {
            return ::SCoro::Poll(*this);
        }
    };

    template <template <typename> class ... Args>
    struct SCoro : Checker<Args...>, Impl::ReverseStages<SCoro<Args...>, Impl::ArgList<Args...>>::type, EboIndex<sizeof...(Args)>
    {
        using checker = Checker<Args...>;
        using base = typename Impl::ReverseStages<SCoro<Args...>, Impl::ArgList<Args...>>::type;
        using index_t =  EboIndex<sizeof...(Args)>;
        using base::base;
        using base::get_at;
        using checker::all_have_base;

        static constexpr size_t count = sizeof...(Args);

        template <typename ... Ags>
        constexpr void Reset(Ags && ... args) noexcept
        {
            this->~SCoro(); 
            new (this) SCoro{std::forward<Ags>(args)...};
        }

        constexpr bool Done() const noexcept
        {
            return index_t::Index() == count;
        }

        constexpr auto & Next() noexcept
        {
            return static_cast<Nexter<SCoro, 1> &>(*this);
        }
        constexpr auto Poll() noexcept
        {
            return ::SCoro::Poll(*this);
        }
    };
}
