#ifndef INCLUDE_AUX_UNIQUE_HANDLE_HPP
#define INCLUDE_AUX_UNIQUE_HANDLE_HPP

#include <concepts>
#include <memory>
#include <cstddef>

namespace aux
{
    namespace impl
    {
        template <class T, T NIL>
        struct pseudo_pointer {
            T handle;

            constexpr pseudo_pointer(pseudo_pointer const&) noexcept = default;
            constexpr pseudo_pointer(pseudo_pointer&&) noexcept = default;
            constexpr pseudo_pointer& operator=(pseudo_pointer const&) = default;
            constexpr pseudo_pointer& operator=(pseudo_pointer&&) = default;

            constexpr pseudo_pointer(T handle) noexcept
                : handle{handle}
            {
            }
            constexpr pseudo_pointer(std::nullptr_t = nullptr) noexcept
                : handle{NIL}
            {
            }
            constexpr T get() const noexcept { return this->handle; }
            constexpr operator T() const noexcept { return this->get(); }
            constexpr explicit operator bool() const noexcept { return this->get() != NIL; }
            constexpr auto operator<=>(pseudo_pointer const&) const noexcept = default;
            constexpr auto operator<=>(std::nullptr_t) const noexcept {
                return *this <=> pseudo_pointer{NIL};
            }
        };
        template <class T, T NIL, class D> requires std::invocable<D, T>
        struct handle_deleter {
            D del;
            using pointer = pseudo_pointer<T, NIL>;
            constexpr handle_deleter(D del) noexcept : del{del}
            {
            }
            constexpr auto operator()(T handle) const noexcept {
                return this->del(handle);
            }
        };

        template <class T, T NIL, auto DEL> requires std::invocable<decltype (DEL), T>
        struct handle_deleter_no_addr {
            using pointer = pseudo_pointer<T, NIL>;
            constexpr auto operator()(T handle) const noexcept {
                return DEL(handle);
            }
        };
    } // ::impl

    template <class T, T NIL, class D> requires std::invocable<D, T>
    struct [[nodiscard]] unique_handle : std::unique_ptr<T, impl::handle_deleter<T, NIL, D>> {
        using base_type = std::unique_ptr<T, impl::handle_deleter<T, NIL, D>>;
        unique_handle(T handle, D del) noexcept
            : base_type{handle, typename base_type::deleter_type{del}}
        {
        }
        constexpr operator T() const noexcept { return base_type::get(); }
    };

    template <class T, T NIL, auto DEL> requires std::invocable<decltype (DEL), T>
    struct [[nodiscard]] unique_handle_no_addr : std::unique_ptr<T, impl::handle_deleter_no_addr<T, NIL, DEL>> {
        using base_type = std::unique_ptr<T, impl::handle_deleter_no_addr<T, NIL, DEL>>;
        unique_handle_no_addr(T handle = NIL) noexcept
            : base_type{handle, typename base_type::deleter_type{}}
        {
        }
        constexpr operator T() const noexcept { return base_type::get(); }
    };
} // ::aux

#endif // INCLUDE_AUX_UNIQUE_HANDLE_HPP
