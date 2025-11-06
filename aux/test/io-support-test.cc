#include <gtest/gtest.h>

#include <aux/io-support.hpp>


class io_support_test : public testing::Test {
protected:
    void SetUp() override { }
    void TearDown() override {}
};

TEST_F(io_support_test, unique_fd) {
    {
        // aux::unique_fd fd{open("test.txt", O_CREAT)};
        // if (!fd) {
        //     fd = open("test.txt", O_APPEND);
        // }
        // char buf[8] = "hi";
        // write(fd, buf, 8);
        // ASSERT_TRUE(fd);
    }
}
