
#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>

#include <aux/packed-tuple.hpp>

#include <spanstream>
#include <string_view>


TEST_CASE("initializers", "[packed_tuple]") {
    {
        constexpr aux::packed_tuple t{3.14, 'c', 42};
        static_assert(sizeof (t) == sizeof (3.14) + sizeof ('c') + sizeof (42));
        static_assert(get<0>(t) == 3.14);
        static_assert(get<1>(t) == 'c');
        static_assert(get<2>(t) == 42);
        REQUIRE(get<0>(t) == 3.14);
        REQUIRE(get<1>(t) == 'c');
        REQUIRE(get<2>(t) == 42);
    }
    {
        constexpr aux::packed_tuple<short, char, int> t;
        static_assert(sizeof (t) == sizeof (short) + sizeof (char) + sizeof (int));
        static_assert(get<0>(t) == 0);
        static_assert(get<1>(t) == '\0');
        static_assert(get<2>(t) == 0);
        REQUIRE(get<0>(t) == 0);
        REQUIRE(get<1>(t) == '\0');
        REQUIRE(get<2>(t) == 0);
    }
}

TEST_CASE("assignments", "[packed_tuple]") {
    {
        constexpr aux::packed_tuple t0{3.14, 'c', 42};
        {
            constexpr auto t{t0};
            static_assert(get<0>(t) == 3.14);
            static_assert(get<1>(t) == 'c');
            static_assert(get<2>(t) == 42);
            REQUIRE(get<0>(t) == 3.14);
            REQUIRE(get<1>(t) == 'c');
            REQUIRE(get<2>(t) == 42);
        }
        aux::packed_tuple t{2.71, 'e', 666};
        t = t0;
        REQUIRE(get<0>(t) == 3.14);
        REQUIRE(get<1>(t) == 'c');
        REQUIRE(get<2>(t) == 42);

        aux::packed_tuple t2 = std::move(t);
        REQUIRE(get<0>(t2) == 3.14);
        REQUIRE(get<1>(t2) == 'c');
        REQUIRE(get<2>(t2) == 42);
    }
}

TEST_CASE("stream inserter", "[packed_tuple]") {
    {
        constexpr aux::packed_tuple t{3.14, 'c', 42};
        std::array<char, 32> buf{};
        std::spanstream output{buf};

        output << t;
        REQUIRE(std::string_view("(3.14 c 42)") == output.span().data());
    }
}

TEST_CASE("structual binding", "[packed_tuple]") {
    {
        constexpr aux::packed_tuple t{3.14, 'c', 42};
        auto [a, b, c] = t;
        REQUIRE(a == 3.14);
        REQUIRE(b == 'c');
        REQUIRE(c == 42);
    }
}
