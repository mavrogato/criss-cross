
#include <iostream>
#include <filesystem>

#include <optional>
#include <cassert>

#include <sys/socket.h>
#include <sys/un.h>

#include <aux/io-support.hpp>

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
} // ::aux

aux::unique_fd connect(std::filesystem::path name = "") noexcept {
    // To find the Unix socket to connect to, most implementations just do what libwayland does:
    // 1. If WAYLAND_SOCKET is set, interpret it as a file descriptor
    //    number on which the connection is already established,
    //    assuming that the parent process configured the connection for us.
    if (auto prepared = aux::from_chars<int>(std::getenv("WAYLAND_SOCKET"))) {
        return {prepared.value()}; // return prepared fd
    }

    // 2. If WAYLAND_DISPLAY is set, concat with XDG_RUNTIME_DIR to form the path to the Unix socket.
    if (name.empty()) {
        name = std::getenv("WAYLAND_DISPLAY");
    }
    // 3. Assume the socket name is wayland-0 and concat with XDG_RUNTIME_DIR to form the path to the Unix socket.
    if (name.empty()) {
        name = "wayland-0";
    }
    if (!name.is_absolute()) {
        auto dir = std::getenv("XDG_RUNTIME_DIR");
        if (!dir) {
            // 4. Give up.
            std::cerr << "error: XDG_RUNTIME_DIR is not set in the environment." << std::endl;
            return {};
        }
        std::filesystem::path runtime = dir;
        if (!runtime.is_absolute()) {
            // 4. Give up.
            std::cerr << "error: XDG_RUNTIME_DIR is invalid in the environment." << std::endl;
            return {};
        }
        name = runtime / name;
    }

    if (sizeof (sockaddr_un::sun_path) < name.string().size()) {
        std::cerr << "error: socket path exceeds sockaddr_un::sun_path." << std::endl;
        errno = ENAMETOOLONG;
        return {};
    }

    aux::unique_fd fd = socket(PF_LOCAL, SOCK_STREAM, 0);
    if (!fd) {
        return {};
    }
    int flags = fcntl(fd, F_GETFD);
    if (-1 == flags) {
        return {};
    }
    if (-1 == fcntl(fd, F_SETFD, flags | FD_CLOEXEC)) {
        return {};
    }

    sockaddr_un addr {
        .sun_family = AF_LOCAL,
        .sun_path = {},
    };
    std::copy(name.string().begin(), name.string().end(), addr.sun_path);
    if (connect(fd, reinterpret_cast<sockaddr*>(&addr),
                offsetof(sockaddr_un, sun_path) + name.string().size()) < 0) {
        return {};
    }
    return fd;
}

#if 0
#include <chrono>
#include <span>

#include <sys/poll.h>

#include <aux/tuple-support.hpp>

int main() {
    using namespace std::chrono_literals;

    if (auto fd = connect()) {
        iovec iov {
            .iov_base = (void*)"\1\0\0\0\0\0\f\0\2\0\0\0",
            .iov_len = 12,
        };
        msghdr head {
            .msg_name = nullptr,
            .msg_namelen = 0,
            .msg_iov = &iov,
            .msg_iovlen = 1,
            .msg_control = nullptr,
            .msg_controllen = 0,
            .msg_flags = 0,
        };
        auto ret = sendmsg(fd, &head, MSG_DONTWAIT | MSG_NOSIGNAL);
        std::cout << ret << std::endl;
        pollfd fds[] {
            {
                .fd = fd,
                .events = POLLIN,
                .revents = {},
            },
        };
        auto piv = std::chrono::steady_clock::now();
        ret = ppoll(fds, 1, nullptr, nullptr);
        auto fin = std::chrono::steady_clock::now();
        std::cout << std::chrono::duration_cast<std::chrono::microseconds>(fin - piv) << std::endl;
        char buf[4096] = {};
        iov = {
            .iov_base = buf,
            .iov_len = sizeof (buf),
        };
        head.msg_flags = MSG_CMSG_CLOEXEC;
        ret = recvmsg(fd, &head, MSG_DONTWAIT | MSG_CMSG_CLOEXEC);
        std::cout << head.msg_iov->iov_len << std::endl;
    }
}
#else

#include <compare>
#include <spanstream>

#include <wayland-client.h>

// #include <embed>
// constexpr auto src = std::embed("wayland-src/protocol/wayland.xml");
// constexpr auto src = {
// #embed "wayland-src/protocol/wayland.xml"
// };
constexpr auto src = []() noexcept {
    uint32_t tmp[] = {
#embed "wayland-src/protocol/wayland.xml"
    };
    std::array<char, std::size(tmp)> ret = {};
    std::transform(std::begin(tmp), std::end(tmp), std::begin(ret),
                   [](auto item) noexcept {
                       return static_cast<char>(item);
                   });
    return ret;
 }();

namespace aux
{
    template <size_t N>
    struct literal : std::array<char, N> {
        constexpr literal(char const (&src)[N]) noexcept
            : std::array<char, N>{}
        {
            static_assert(N > 1);
            std::copy(std::begin(src), std::end(src), this->begin());
        }
        constexpr operator std::string_view() const noexcept { return {this->data()}; }
        constexpr auto size() const noexcept { return N-1; }

        template <class Ch, class Tr>
        friend auto& operator<<(std::basic_ostream<Ch, Tr>& output, literal const& x) {
            return output << static_cast<std::string_view>(x);
        }
    };

    template <size_t N>
    struct node : std::span<char, N> {
        constexpr node(std::span<char, N> src)
            : std::span<char, N>{src}
        {
        }
        template <class I>
        constexpr node(I first, size_t count)
            : std::span<I>{first, count}
        {
        }
        template <char X>
        static constexpr auto token(node s) noexcept {
            return X == s.front() ? node{s.begin(), 1} : node{s.begin(), 0};
        }
    };

    template <char E>
    constexpr auto match(auto src) noexcept {
        return E == src.front() ? std::span{src.begin(), 1} : std::span{src.begin(), 0};
    }
    template <literal E, size_t N = E.size()>
    constexpr auto match(auto src) noexcept {
        return E == std::string_view{src.begin(), src.begin()+N} ? std::span{src.begin(), N} : std::span{src.begin(), 0};
    }

    template <class... Args>
    constexpr auto alter(Args... args) noexcept {
        return [args...](auto src) noexcept {
            auto ret = (!args(src).empty() || ...);
        };
    }

    constexpr auto prolog(std::span<char> src) noexcept {
        std::spanstream ss(src);
    }
}

int main() {
    //auto fd = connect("wayland-0");
    if (auto display = wl_display_connect(nullptr)) {
        wl_display_roundtrip(display);
        if (auto registry = wl_display_get_registry(display)) {
            wl_registry_listener listener = {
                .global = [](auto...) noexcept {
                    std::cout << "*";
                },
                .global_remove = [](auto...) noexcept {
                },
            };
            wl_registry_add_listener(registry, &listener, nullptr);
            wl_display_roundtrip(display);
            wl_registry_destroy(registry);
        }
        wl_display_disconnect(display);
        std::cout << "\n-------" << std::endl;

        auto ret = aux::match<"<?xml">(std::span{src});
        for (auto item : ret) {
            std::cout << item << std::endl;
        }

        std::cout << "\n-------" << std::endl;
        std::cout << aux::literal{"Hello"} << std::endl;
    }
    return 0;
}
#endif
