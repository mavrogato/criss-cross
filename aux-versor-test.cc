
#include <gtest/gtest.h>

#include <aux/versor.hpp>

#include <array>
#include <spanstream>

class aux_versor_test : public testing::Test {
protected:
    void SetUp() override { }
    void TearDown() override {}
};

TEST_F(aux_versor_test, initializer) {
    {
        constexpr aux::versor<int, 3> v{1,2,3};
        static_assert(get<0>(v) == 1);
        static_assert(get<1>(v) == 2);
        static_assert(get<2>(v) == 3);
        ASSERT_EQ(get<0>(v), 1);
        ASSERT_EQ(get<1>(v), 2);
        ASSERT_EQ(get<2>(v), 3);
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
        ASSERT_TRUE(get<0>(v) == 0);
        ASSERT_TRUE(get<1>(v) == 0);
        ASSERT_TRUE(get<2>(v) == 0);
        ASSERT_TRUE(get<3>(v) == 0);
        ASSERT_TRUE(get<4>(v) == 0);
        ASSERT_TRUE(get<5>(v) == 0);
        ASSERT_TRUE(get<6>(v) == 0);
        ASSERT_TRUE(get<7>(v) == 0);
    }
}

TEST_F(aux_versor_test, copying) {
    {
    }
}

TEST_F(aux_versor_test, structual_binding) {
    {
        aux::versor<uint8_t, 3> v{0, 1, 2};
        auto [a, b, c] = v;
        ASSERT_TRUE(a == 0);
        ASSERT_TRUE(b == 1);
        ASSERT_TRUE(c == 2);
    }
}

TEST_F(aux_versor_test, indexer) {
    {
        aux::versor<double, 4> v{1.1, 2.2, 3.3, 4.4};
        ASSERT_EQ(v[0], 1.1);
        ASSERT_EQ(v[1], 2.2);
        ASSERT_EQ(v[2], 3.3);
        ASSERT_EQ(v[3], 4.4);
    }
}

TEST_F(aux_versor_test, checked_indexer) {
    {
        aux::versor<bool, 2> v{false, true};
        ASSERT_EQ(v.at(0), get<0>(v));
        ASSERT_EQ(v.at(1), get<1>(v));
        ASSERT_EQ(v[0], false);
        ASSERT_EQ(v[1], true);

        try {
            v.at(-1);
            ASSERT_TRUE(false);
        }
        catch (std::range_error) {
            ASSERT_TRUE(true);
        }
        try {
            v.at(2);
            ASSERT_TRUE(false);
        }
        catch (std::range_error) {
            ASSERT_TRUE(true);
        }

    }
}

TEST_F(aux_versor_test, list_format) {
    {
        aux::versor<char, 5> v{'a', 'b', 'c', 'd', 'e'};
        std::array<char, 32> buf{};
        std::spanstream output{buf};
        output << v;
        ASSERT_STREQ("(a b c d e)", output.span().data());
    }
}

TEST_F(aux_versor_test, vectorized_algebra) {
    {
        constexpr aux::versor<double, 3> v{-1.0, 0.5, 2.0};
        static_assert(get<0>(v+v) == -2.0);
        static_assert(get<1>(v-v) == 0.0);
        static_assert(get<2>(v*v) == 4.0);
        static_assert(get<0>(v/v) == 1.0);

        ASSERT_TRUE(get<0>(v+v) == -2.0);
        ASSERT_TRUE(get<1>(v-v) == 0.0);
        ASSERT_TRUE(get<2>(v*v) == 4.0);
        ASSERT_TRUE(get<0>(v/v) == 1.0);
    }
}

TEST_F(aux_versor_test, vectorized_bitwise_operation) {
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

        ASSERT_TRUE(v0 == (v1^v1));
        ASSERT_TRUE(~v1 == v2);
        ASSERT_TRUE(v1 == ~v2);
        ASSERT_TRUE(v3 == (v1|v2));
        ASSERT_TRUE(v2 == (v2&v3));
        ASSERT_TRUE(v1 == (v2^v3));
    }
}

TEST_F(aux_versor_test, relation) {
    {
        constexpr aux::versor<int, 2> v1{0, 0};
        constexpr aux::versor<int, 2> v2{0, 1};
        constexpr aux::versor<int, 2> v3{1, 0};
        constexpr aux::versor<int, 2> v4{1, 1};

        static_assert(v1 < v2);
        static_assert(v2 < v3);
        static_assert(v3 < v4);
        static_assert(v1 != v2);
        static_assert(v2 != v3);
        static_assert(v3 != v4);
        static_assert(v1 == v1);
        static_assert(v2 == v2);
        static_assert(v3 == v3);
        static_assert(v3 == v3);
        static_assert((v2+v3) == v4);
        static_assert(v1 <=> v2 < 0);
        static_assert(v2 <=> v1 > 0);
        static_assert(v3 <=> v3 == 0);

        ASSERT_TRUE(v1 < v2);
        ASSERT_TRUE(v2 < v3);
        ASSERT_TRUE(v3 < v4);
        ASSERT_TRUE(v1 != v2);
        ASSERT_TRUE(v2 != v3);
        ASSERT_TRUE(v3 != v4);
        ASSERT_TRUE(v1 == v1);
        ASSERT_TRUE(v2 == v2);
        ASSERT_TRUE(v3 == v3);
        ASSERT_TRUE(v3 == v3);
        ASSERT_TRUE((v2+v3) == v4);
        ASSERT_TRUE(v1 <=> v2 < 0);
        ASSERT_TRUE(v2 <=> v1 > 0);
        ASSERT_TRUE(v3 <=> v3 == 0);
    }
}

TEST_F(aux_versor_test, linear_algebra) {
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

        ASSERT_TRUE(v*4.0 == 4.0*v);
        ASSERT_TRUE(v*1.0 == v/1.0);
        ASSERT_TRUE(v*2.0 == v+v);
        ASSERT_TRUE(get<0>(v*3.0) == 3);
        ASSERT_TRUE(get<1>(v*3.0) == 6);
        ASSERT_TRUE(get<2>(v*3.0) == 9);
        ASSERT_TRUE(get<3>(v*3.0) == 12);
        ASSERT_TRUE(get<0>(v/2.0) == 0.5);
        ASSERT_TRUE(get<1>(v/2.0) == 1.0);
        ASSERT_TRUE(get<2>(v/2.0) == 1.5);
        ASSERT_TRUE(get<3>(v/2.0) == 2.0);
    }
}
