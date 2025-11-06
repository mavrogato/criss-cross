#ifndef INCLUDE_AUX_PACKED_TUPLE_HPP
#define INCLUDE_AUX_PACKED_TUPLE_HPP

#include <concepts>
#include <iosfwd>
#include <tuple>

#include <aux/tuple-support.hpp>

namespace aux::inline tuple_support
{
    template <class First, class... Rest>
    class packed_tuple {
    public:
        constexpr packed_tuple(packed_tuple const&) = default;
        constexpr packed_tuple(packed_tuple&&) = default;
        constexpr packed_tuple& operator=(packed_tuple const&) = default;
        constexpr packed_tuple& operator=(packed_tuple&&) = default;
        constexpr auto operator<=>(packed_tuple const&) const = default;

        constexpr packed_tuple() noexcept
            : first{}
            , rest{}
        {
        }

        constexpr packed_tuple(First first, Rest... rest) noexcept
            : first{first}
            , rest{rest...}
        {
        }

    public:
        template <size_t N>
        constexpr auto get() const noexcept {
            if constexpr (N == 0) {
                return this->first;
            }
            else {
                return rest.template get<N-1>();
            }
        }
        template <size_t N>
        constexpr auto& get() noexcept {
            if constexpr (N == 0) {
                return this->first;
            }
            else {
                return rest.template get<N-1>();
            }
        }

    private:
        First first;
        packed_tuple<Rest...> rest;
    } __attribute__((__packed__));

    template <class First>
    class packed_tuple<First> {
    public:
        constexpr packed_tuple(packed_tuple const&) = default;
        constexpr packed_tuple(packed_tuple&&) = default;
        constexpr packed_tuple& operator=(packed_tuple const&) = default;
        constexpr packed_tuple& operator=(packed_tuple&&) = default;
        constexpr auto operator<=>(packed_tuple const&) const = default;

    public:
        constexpr packed_tuple(First first = First()) noexcept
            : first{first}
        {
        }

        template <size_t N>
        constexpr auto get() const noexcept {
            static_assert(N==0);
            return this->first;
        }
        template <size_t N>
        constexpr auto& get() noexcept {
            static_assert(N==0);
            return this->first;
        }

    private:
        First first;
    } __attribute__((__packed__));

    template <size_t I, class... Args>
    constexpr auto get(packed_tuple<Args...> const& src) noexcept {
        return src.template get<I>();
    }
    template <size_t I, class... Args>
    constexpr auto& get(packed_tuple<Args...>& src) noexcept {
        return src.template get<I>();
    }
} // namespace aux::inline tuple_support

namespace std
{
    template <class... Args>
    struct tuple_size<aux::tuple_support::packed_tuple<Args...>> {
        static constexpr auto value = sizeof... (Args);
    };
    template <class... Args>
    constexpr size_t tuple_size_v<aux::tuple_support::packed_tuple<Args...>> = sizeof... (Args);
    template <size_t I, class... Args>
    struct tuple_element<I, aux::tuple_support::packed_tuple<Args...>> {
        using type = decltype (std::declval<aux::tuple_support::packed_tuple<Args...>>().template get<I>());
    };
}

#endif // INCLUDE_AUX_PACKED_TUPLE_HPP
