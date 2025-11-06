
#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>

#include <aux/tuple-support.hpp>

#include <array>
#include <spanstream>
#include <string_view>
#include <tuple>


TEST_CASE("stream inserter", "[tuple_support]") {
    {
        using aux::operator<<;

        constexpr auto t = std::tuple{3.14, 'c', 42};
        auto buf = std::array<char, 32>{};
        auto out = std::spanstream{buf};
        out << t;
        REQUIRE(std::string_view(out.span().data()) == "(3.14 c 42)");
    }
}
