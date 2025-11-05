#ifndef INCLUDE_AUX_VERSOR_HPP
#define INCLUDE_AUX_VERSOR_HPP

#include <cstddef>
#include <utility>
#include <iosfwd>
#include <functional>
#include <stdexcept>

#include <aux/tuple-support.hpp>

namespace aux
{
    template <class T, size_t N>
    struct versor : versor<T, N-1> {
    public:
        using base_type = versor<T, N-1>;
        using value_type = typename base_type::value_type;
        using iterator = typename base_type::iterator;
        using const_iterator = typename base_type::const_iterator;
        using reference = typename base_type::reference;
        using const_reference = typename base_type::const_reference;

    public:
        constexpr friend size_t size(versor) noexcept { return N; }
        constexpr size_t size() const noexcept { return N; }

    public:
        value_type last;

    public:
        constexpr versor(versor const&) = default;
        constexpr versor(versor&&) = default;
        constexpr versor& operator=(versor const&) = default;
        constexpr versor& operator=(versor&&) = default;
        constexpr auto operator<=>(versor const& rhs) const noexcept = default;

    public:
        constexpr versor(auto... args) noexcept
        : versor{{static_cast<T>(args)...}, std::make_index_sequence<N-1>()}
        {
            static_assert(sizeof... (args) <= N);
        }

    private:
        constexpr static auto at(std::initializer_list<T> const& args, size_t i) noexcept {
            return (i < args.size()) ? *(args.begin() + i) : T();
        }
        template <size_t... I>
        constexpr versor(std::initializer_list<T>&& args, std::index_sequence<I...>) noexcept
            : base_type{at(args, I)...}, last{at(args, N-1)}
        {
        }

    public:
        template <size_t I>
        constexpr auto get() const noexcept {
            static_assert(I < N);
            return static_cast<versor<T, I+1> const*>(this)->last;
        }
        template <size_t I>
        constexpr auto& get() noexcept {
            static_assert(I < N);
            return static_cast<versor<T, I+1>*>(this)->last;
        }
        template <size_t I>
        constexpr friend auto get(versor const& v) noexcept { return v.get<I>(); }
        template <size_t I>
        constexpr friend auto& get(versor& v) noexcept { return v.get<I>(); }

    public:
        constexpr auto begin() const noexcept { return &static_cast<versor<T, 1> const*>(this)->last; }
        constexpr auto begin() noexcept { return &static_cast<versor<T, 1>*>(this)->last; }
        constexpr auto end() const noexcept { return &this->last + 1; }
        constexpr auto end() noexcept { return &this->last + 1; }

        constexpr auto front() const noexcept { return *(this->begin()); }
        constexpr auto& front() noexcept { return *(this->begin()); }
        constexpr auto back() const noexcept { return this->last; }
        constexpr auto& back() noexcept { return this->last; }

        auto& operator[](size_t i) noexcept { return *(begin() + i); }
        auto operator[](size_t i) const noexcept { return *(begin() + i); }

        auto& at(size_t i) {
            if (this->size() <= i)
                throw std::range_error("versor index");
            return (*this)[i];
        }
        auto at(size_t i) const {
            if (this->size() <= i)
                throw std::range_error("versor index");
            return (*this)[i];
        }

    public:
        template <class Func, class... Rest>
        constexpr auto& apply(Func&& func, Rest&&... rest) noexcept {
            this->last = func(this->last, rest.last...);
            if constexpr ((std::is_rvalue_reference_v<Rest> && ...)) {
                base_type::apply(std::forward<Func>(func), std::move(static_cast<base_type&>(rest))...);
            }
            else {
                base_type::apply(std::forward<Func>(func), static_cast<base_type const&>(rest)...);
            }
            return *this;
        }

        constexpr auto& negate() noexcept { return apply(std::negate<T>()); }
        constexpr auto& lognot() noexcept { return apply(std::bit_not<T>()); }

    public:
        constexpr auto operator+() const noexcept { return *this; }
        constexpr auto operator-() const noexcept { return (+(*this)).negate(); }

        constexpr auto& operator+=(auto&& rhs) noexcept { return apply(std::plus<T>(), rhs); }
        constexpr auto& operator-=(auto&& rhs) noexcept { return apply(std::minus<T>(), rhs); }
        constexpr auto& operator*=(auto&& rhs) noexcept { return apply(std::multiplies<T>(), rhs); }
        constexpr auto& operator/=(auto&& rhs) noexcept { return apply(std::divides<T>(), rhs); }

        constexpr auto operator+(auto&& rhs) const noexcept { return (+(*this)) += rhs; }
        constexpr auto operator-(auto&& rhs) const noexcept { return (+(*this)) -= rhs; }
        constexpr auto operator*(auto&& rhs) const noexcept { return (+(*this)) *= rhs; }
        constexpr auto operator/(auto&& rhs) const noexcept { return (+(*this)) /= rhs; }

        constexpr auto& operator*=(value_type s) noexcept {
            return apply([s](value_type x) noexcept {
                return x * s;
            });
        }
        constexpr auto operator*(value_type s) const noexcept { return (+(*this)) *= s; }
        constexpr friend auto operator*(value_type s, versor v) noexcept { return v * s; }
        constexpr auto& operator/=(value_type s) noexcept { return (*this) *= (1/s); }
        constexpr auto operator/(value_type s) noexcept { return (+(*this)) /= s; }

        constexpr auto operator~() const noexcept { return (+(*this)).lognot(); }

        constexpr auto& operator^=(auto&& rhs) noexcept { return apply(std::bit_xor<T>(), rhs); }
        constexpr auto& operator|=(auto&& rhs) noexcept { return apply(std::bit_or<T>(), rhs); }
        constexpr auto& operator&=(auto&& rhs) noexcept { return apply(std::bit_and<T>(), rhs); }

        constexpr auto operator^(auto&& rhs) const noexcept { return (+(*this)) ^= rhs; }
        constexpr auto operator|(auto&& rhs) const noexcept { return (+(*this)) |= rhs; }
        constexpr auto operator&(auto&& rhs) const noexcept { return (+(*this)) &= rhs; }
    };

    template <class T>
    struct versor<T, 0> {
    public:
        using value_type = T;
        using iterator = value_type*;
        using const_iterator = value_type const*;
        using reference = value_type&;
        using const_reference = value_type const&;
        using size_type = size_t;

        constexpr auto operator<=>(versor const& rhs) const noexcept = default;

    protected:
        constexpr auto& apply(auto&&...) noexcept { return *this; }
        constexpr friend T inner(versor const&, versor const&) noexcept { return T(); }
    };
} // ::aux

// WIP. experimental tuple support
namespace std
{
    template <class T, size_t N>
    struct tuple_size<aux::versor<T, N>> {
        static constexpr auto value = N;
    };
    template <class T, size_t N>
    constexpr size_t tuple_size_v<aux::versor<T, N>> = tuple_size<aux::versor<T, N>>::value;

    template <size_t I, class T, size_t N>
    struct tuple_element<I, aux::versor<T, N>> {
        using type = T;
    };
} // namespace std



#endif // INCLUDE_AUX_VERSOR_HPP
