
#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>

#include <aux/versor.hpp>

#include <cstddef>
#include <sstream>
#include <string>
#include <algorithm>


TEST_CASE("memory layout", "[versor_test]") {
    {
        constexpr aux::versor<double, 0> nil;
        static_assert(nil.size() == 0);
        static_assert(sizeof (nil) == 1);
        REQUIRE(nil.size() == 0);
        REQUIRE(sizeof (nil) == 1);
    }
    {
        constexpr aux::versor<double, 32> v;
        static_assert(v.size() == 32);
        static_assert(v.size() * sizeof (double) == sizeof (v));
        REQUIRE(v.size() == 32);
        REQUIRE(v.size() * sizeof (double) == sizeof (v));
    }
}

TEST_CASE("initializer", "[versor_test]") {
    {
        constexpr aux::versor<double, 2> v{};
        static_assert(get<0>(v) == 0);
        static_assert(get<1>(v) == 0);
        REQUIRE(get<0>(v) == 0);
        REQUIRE(get<1>(v) == 0);
    }
    {
        constexpr aux::versor<int, 3> v{1,2,3};
        static_assert(get<0>(v) == 1);
        static_assert(get<1>(v) == 2);
        static_assert(get<2>(v) == 3);
        REQUIRE(get<0>(v) == 1);
        REQUIRE(get<1>(v) == 2);
        REQUIRE(get<2>(v) == 3);
    }
    {
        constexpr aux::versor<int, 8> v;
        static_assert(get<0>(v) == 0);
        static_assert(get<1>(v) == 0);
        static_assert(get<2>(v) == 0);
        static_assert(get<3>(v) == 0);
        static_assert(get<4>(v) == 0);
        static_assert(get<5>(v) == 0);
        static_assert(get<6>(v) == 0);
        static_assert(get<7>(v) == 0);
        REQUIRE(get<0>(v) == 0);
        REQUIRE(get<1>(v) == 0);
        REQUIRE(get<2>(v) == 0);
        REQUIRE(get<3>(v) == 0);
        REQUIRE(get<4>(v) == 0);
        REQUIRE(get<5>(v) == 0);
        REQUIRE(get<6>(v) == 0);
        REQUIRE(get<7>(v) == 0);
    }
}

TEST_CASE("assignment", "[versor_test]") {
    {
        constexpr aux::versor<int, 5> a{1,2,3,4,5};
        auto b = a;
        REQUIRE(b == a);
        auto c = std::move(b);
        REQUIRE(c == a);
        c = aux::versor<int, 5>{};
        REQUIRE(std::all_of(c.begin(), c.end(), [](auto x) noexcept {
            return x == 0;
        }));
        c = a;
        REQUIRE(c == a);
    }
}

TEST_CASE("structual_binding", "[versor_test]") {
    {
        aux::versor<uint8_t, 3> v{0, 1, 2};
        auto& [a, b, c] = v;
        REQUIRE(a == 0);
        REQUIRE(b == 1);
        REQUIRE(c == 2);
        a = 9;
        b = 8;
        c = 7;
        auto ret = v == aux::versor<uint8_t, 3>{9, 8, 7};
        REQUIRE(ret);
    }
}

TEST_CASE("indexer", "[versor_test]") {
    {
        constexpr aux::versor<double, 4> v{1.1, 2.2, 3.3, 4.4};
        REQUIRE(aux::almost_equal(v[0], 1.1));
        REQUIRE(aux::almost_equal(v[1], 2.2));
        REQUIRE(aux::almost_equal(v[2], 3.3));
        REQUIRE(aux::almost_equal(v[3], 4.4));
    }
}

TEST_CASE("checked indexer", "[versor_test]") {
    {
        aux::versor<bool, 2> v{false, true};
        REQUIRE(v[0] == false);
        REQUIRE(v[1] == true);
        REQUIRE_NOTHROW(v.at(0));
        REQUIRE_NOTHROW(v.at(1));
        REQUIRE_THROWS_AS(v.at(-1), std::range_error);
        REQUIRE_THROWS_AS(v.at(2), std::range_error);
        REQUIRE(v[0] == v.at(0));
        REQUIRE(v[1] == v.at(1));
    }
}

TEST_CASE("list format", "[versor_test]") {
    {
        aux::versor<char, 5> v{'a', 'b', 'c', 'd', 'e'};
        std::stringstream output;
        output << v;
        REQUIRE("(a b c d e)" == output.str());
    }
}

TEST_CASE("vectorized algebra", "[versor_test]") {
    {
        constexpr aux::versor<double, 3> v{-1.0, 0.5, 2.0};
        static_assert(get<0>(v+v) == -2.0);
        static_assert(get<1>(v-v) == 0.0);
        static_assert(get<2>(v*v) == 4.0);
        static_assert(get<0>(v/v) == 1.0);

        // Note: These equalities are not guaranteed to be exact at runtime due to floating-point precision
        REQUIRE((get<0>(v+v)) == -2.0);
        REQUIRE((get<1>(v-v)) == 0.0);
        REQUIRE((get<2>(v*v)) == 4.0);
        REQUIRE((get<0>(v/v)) == 1.0);
    }
}

TEST_CASE("vectorized bitwise operation", "[versor_test]") {
    {
        constexpr aux::versor<uint16_t, 2> v0{0x0000, 0x0000};
        constexpr aux::versor<uint16_t, 2> v1{0x00ff, 0xff00};
        constexpr aux::versor<uint16_t, 2> v2{0xff00, 0x00ff};
        constexpr aux::versor<uint16_t, 2> v3{0xffff, 0xffff};

        static_assert(v0 == (v1^v1));
        static_assert(~v1 == v2);
        static_assert(v1 == ~v2);
        static_assert(v3 == (v1|v2));
        static_assert(v2 == (v2&v3));
        static_assert(v1 == (v2^v3));

        REQUIRE(v0 == (v1^v1));
        REQUIRE(~v1 == v2);
        REQUIRE(v1 == ~v2);
        REQUIRE(v3 == (v1|v2));
        REQUIRE(v2 == (v2&v3));
        REQUIRE(v1 == (v2^v3));
    }
}

TEST_CASE("partial apply", "[versor_test]") {
    aux::versor<double, 3> v{1, 2, 3};
    v += aux::versor<double, 2>{10, 20};
    INFO("v = " << v);
    REQUIRE((v == aux::versor<double, 3>{11, 22, 3}));
}

TEST_CASE("apply sick normalize", "[versor_test]") {
    aux::versor<double, 3> v{1,2,3};
    v.apply([w = v[2]](auto x) noexcept { return x / w; });
    REQUIRE((v == aux::versor<double, 3>{1.0/3.0, 2.0/3.0, 1.0}));
}

TEST_CASE("floating point equality", "[versor_test]") {
    // strict equality
    REQUIRE(aux::almost_equal(1.0f, 1.0f));
    REQUIRE(aux::almost_equal(-0.0f, 0.0f));

    constexpr auto N = aux::default_ulp_tolerance<float>::value;
    float next = 1.0f;
    for (size_t i = 0; i <= N; ++i) {
        next = std::nextafter(next, 2.0f);
        if (i != N) {
            REQUIRE(aux::almost_equal(1.0f, next));
        }
        else {
            REQUIRE_FALSE(aux::almost_equal(1.0f, next));
        }
    }

    float inf = std::numeric_limits<float>::infinity();
    float nan = std::numeric_limits<float>::quiet_NaN();
    REQUIRE(aux::almost_equal(inf, inf));
    REQUIRE_FALSE(aux::almost_equal(inf, -inf));
    REQUIRE_FALSE(aux::almost_equal(nan, nan));
    REQUIRE_FALSE(aux::almost_equal(1.0f, inf));
}

TEST_CASE("versor equality", "[versor_test]") {
    {
        constexpr aux::versor<double, 2> v1{0, 0};
        constexpr aux::versor<double, 2> v2{0, 1};
        constexpr aux::versor<double, 2> v3{1, 0};
        constexpr aux::versor<double, 2> v4{1, 1};

        static_assert(v1 != v2);
        static_assert(v2 != v3);
        static_assert(v3 != v4);
        static_assert(v1 == v1);
        static_assert(v2 == v2);
        static_assert(v3 == v3);
        static_assert(v3 == v3);
        static_assert((v2+v3) == v4);

        REQUIRE(v1 != v2);
        REQUIRE(v2 != v3);
        REQUIRE(v3 != v4);
        REQUIRE(v1 == v1);
        REQUIRE(v2 == v2);
        REQUIRE(v3 == v3);
        REQUIRE(v3 == v3);
        REQUIRE((v2+v3) == v4);
    }
}

TEST_CASE("linear algebra", "[versor_test]") {
    {
        constexpr aux::versor<double, 4> v{1,2,3,4};
        static_assert(v*4.0 == 4.0*v);
        static_assert(v*1.0 == v/1.0);
        static_assert(v*2.0 == v+v);
        static_assert(get<0>(v*3.0) == 3);
        static_assert(get<1>(v*3.0) == 6);
        static_assert(get<2>(v*3.0) == 9);
        static_assert(get<3>(v*3.0) == 12);
        static_assert(get<0>(v/2.0) == 0.5);
        static_assert(get<1>(v/2.0) == 1.0);
        static_assert(get<2>(v/2.0) == 1.5);
        static_assert(get<3>(v/2.0) == 2.0);

        REQUIRE(v*4.0 == 4.0*v);
        REQUIRE(v*1.0 == v/1.0);
        REQUIRE(v*2.0 == v+v);
        REQUIRE((get<0>(v*3.0)) == 3.0);
        REQUIRE((get<1>(v*3.0)) == 6);
        REQUIRE((get<2>(v*3.0)) == 9);
        REQUIRE((get<3>(v*3.0)) == 12);
        REQUIRE((get<0>(v/2.0)) == 0.5);
        REQUIRE((get<1>(v/2.0)) == 1.0);
        REQUIRE((get<2>(v/2.0)) == 1.5);
        REQUIRE((get<3>(v/2.0)) == 2.0);
    }
}
