#ifndef INCLUDE_AUX_IO_SUPPORT_HPP
#define INCLUDE_AUX_IO_SUPPORT_HPP

#include <span>
#include <utility>

#include <cassert>

#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

#include <aux/unique-handle.hpp>

namespace aux
{
    using unique_fd = aux::unique_handle_no_addr<int, -1, ::close>;

    template <class T>
    class [[nodiscard]] unique_mmap : public std::span<T> {
        using base_type = std::span<T>;
        constexpr auto& base() const noexcept { return static_cast<base_type&>(*this); }

    public:
        constexpr unique_mmap(unique_mmap const&) = delete;
        constexpr unique_mmap& operator=(unique_mmap const&) = delete;

        explicit unique_mmap(int fd,
                             size_t length,
                             int prot = PROT_READ | PROT_WRITE,
                             int flags = MAP_SHARED,
                             size_t offset = 0,
                             void* target = nullptr) noexcept
            : base_type{}
        {
            if (void* addr = ::mmap(target,
                                    length * sizeof (T),
                                    prot,
                                    flags,
                                    fd,
                                    offset * sizeof (T)); addr != MAP_FAILED) {
                static_cast<base_type&>(*this) = {static_cast<T*>(addr), length};
            }
        }
        unique_mmap(unique_mmap&& other) noexcept
            : base_type{std::exchange(other.base(), std::span<T>{})}
        {
        }
        ~unique_mmap() noexcept {
            if (!this->empty() && this->data() != MAP_FAILED) {
                [[maybe_unused]] auto ret = ::munmap(this->data(), this->size() * sizeof (T));
                assert(ret != -1);
            }
        }

        auto& operator=(unique_mmap&& other) noexcept {
            std::exchange(this->base(), std::exchange(other.base(), {}));
            // TODO: ensure destroying...
            return *this;
        }

    public:
        operator T const*() const noexcept { return this->data(); }
        operator T*() noexcept { return this->data(); }
        explicit operator bool() const noexcept { return !this->empty(); }
    };
} // ::aux

#endif // INCLUDE_AUX_IO_SUPPORT_HPP
