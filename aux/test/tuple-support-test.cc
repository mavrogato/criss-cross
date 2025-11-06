#include <gtest/gtest.h>

#include <aux/tuple-support.hpp>

#include <array>
#include <spanstream>

class tuple_support_test : public testing::Test {
protected:
    void SetUp() override { }
    void TearDown() override {}
};

TEST_F(tuple_support_test, lisp_like_format) {
    {
        using aux::operator<<;

        constexpr auto t = std::tuple{3.14, 'c', 42};
        auto buf = std::array<char, 32>{};
        auto out = std::spanstream{buf};
        out << t;
        ASSERT_STREQ(out.span().data(), "(3.14 c 42)");
    }
}
