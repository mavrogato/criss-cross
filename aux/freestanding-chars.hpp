#ifndef INCLUDE_AUX_FREESTANDING_CHARS_HPP
#define INCLUDE_AUX_FREESTANDING_CHARS_HPP

#include <string_view>
#include <charconv>
#include <optional>
#include <algorithm>

namespace aux
{
    template <class T>
    constexpr std::optional<T> from_chars(char const* text) noexcept {
        if (text) {
            std::string_view view(text);
            T result = {};
            if (auto [p, e] = std::from_chars(view.begin(), view.end(), result); e == std::errc{}) {
                return result;
            }
        }
        return std::nullopt;
    }

    template <size_t N>
    constexpr size_t concat(char (&buf)[N], auto... args) noexcept {
        static_assert(0 < N);
        size_t i = 0;
        for (auto arg: {std::string_view(args)...}) {
            if (i + std::size(arg) < N-1) {
                std::copy(std::begin(arg), std::end(arg), buf + i);
                i += std::size(arg);
            }
            else {
                std::copy_n(std::begin(arg), N-1-i, buf + i);
                return N-1;
            }
        }
        return i;
    }

} // ::aux

#endif // INCLUDE_AUX_FREESTANDING_CHARS_HPP
