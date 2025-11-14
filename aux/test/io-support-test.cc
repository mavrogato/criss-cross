#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>

#include <aux/io-support.hpp>

TEST_CASE("io_support_test", "[unique_fd]") {
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
