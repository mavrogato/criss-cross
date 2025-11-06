#include <gtest/gtest.h>

#include <aux/packed-tuple.hpp>

#include <spanstream>


class aux_tuple_test : public testing::Test {
protected:
    void SetUp() override { }
    void TearDown() override {}
};

TEST_F(aux_tuple_test, initializer) {
    {
        constexpr aux::packed_tuple t{3.14, 'c', 42};
        static_assert(sizeof (t) == sizeof (3.14) + sizeof ('c') + sizeof (42));
        static_assert(get<0>(t) == 3.14);
        static_assert(get<1>(t) == 'c');
        static_assert(get<2>(t) == 42);
        ASSERT_TRUE(get<0>(t) == 3.14);
        ASSERT_TRUE(get<1>(t) == 'c');
        ASSERT_TRUE(get<2>(t) == 42);
    }
    {
        constexpr aux::packed_tuple<short, char, int> t;
        static_assert(sizeof (t) == sizeof (short) + sizeof (char) + sizeof (int));
        static_assert(get<0>(t) == 0);
        static_assert(get<1>(t) == '\0');
        static_assert(get<2>(t) == 0);
        ASSERT_TRUE(get<0>(t) == 0);
        ASSERT_TRUE(get<1>(t) == '\0');
        ASSERT_TRUE(get<2>(t) == 0);
    }
}

TEST_F(aux_tuple_test, copying) {
    {
        constexpr aux::packed_tuple t0{3.14, 'c', 42};
        {
            constexpr auto t{t0};
            static_assert(get<0>(t) == 3.14);
            static_assert(get<1>(t) == 'c');
            static_assert(get<2>(t) == 42);
            ASSERT_TRUE(get<0>(t) == 3.14);
            ASSERT_TRUE(get<1>(t) == 'c');
            ASSERT_TRUE(get<2>(t) == 42);
        }
        aux::packed_tuple t{2.71, 'e', 666};
        t = t0;
        ASSERT_TRUE(get<0>(t) == 3.14);
        ASSERT_TRUE(get<1>(t) == 'c');
        ASSERT_TRUE(get<2>(t) == 42);

        aux::packed_tuple t2 = std::move(t);
        ASSERT_TRUE(get<0>(t2) == 3.14);
        ASSERT_TRUE(get<1>(t2) == 'c');
        ASSERT_TRUE(get<2>(t2) == 42);
    }
}

TEST_F(aux_tuple_test, format) {
    {
        constexpr std::tuple t{1, 2, 3};
        std::array<char, 32> buf{};
        std::spanstream output{buf};

        output << t;
        ASSERT_STREQ("(1 2 3)", output.span().data());
    }
    {
        constexpr aux::packed_tuple t{3.14, 'c', 42};
        std::array<char, 32> buf{};
        std::spanstream output{buf};

        output << t;
        ASSERT_STREQ("(3.14 c 42)", output.span().data());
    }
}

TEST_F(aux_tuple_test, structual_binding) {
    // {
    //     constexpr auto arr = std::array{1,2,3};
    //     auto [x, y, z] = arr;
    //     ASSERT_TRUE(x == 1);
    //     ASSERT_TRUE(y == 2);
    //     ASSERT_TRUE(z == 3);
    // }
    {
        constexpr aux::packed_tuple t{3.14, 'c', 42};
        auto [a, b, c] = t;
        ASSERT_TRUE(a == 3.14);
        ASSERT_TRUE(b == 'c');
        ASSERT_TRUE(c == 42);
    }
}
