#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>

#include <aux/freestanding-chars.hpp>


TEST_CASE("conversion test", "[freestanding_chars]") {
    {
        if constexpr (constexpr auto x = aux::from_chars<int>("42 24")) {
            static_assert(x == 42);
        }
    }
}

TEST_CASE("concatenation test", "[freestanding_chars]") {
    {
        char buf[16] = {};
        std::string_view str{buf};

        REQUIRE(str == "");
        aux::concat(buf, "Hello", "World");
        REQUIRE(buf == std::string_view("HelloWorld"));
    }
}
