
cmake_minimum_required(VERSION 3.28)

set(CMAKE_EXPORT_COMILE_COMMANDS ON)

include_directories(
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR})

# C
set(CMAKE_C_COMPILER clang)
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_FLAGS "-Wall -Wextra -save-temps")

# C++
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -fsycl -fsycl-targets=nvptx64-nvidia-cuda -save-temps")

# wayland protocols
include(FeatureSummary)
set_package_properties(WaylandProtocols PROPERTIES
  URL "https://cgit.freedesktop.org/wayland/wayland-protocols"
  DESCRIPTION "current wayland protocols")
unset(WAYLAND_PROTOCOLS_PATH)
find_package(PkgConfig)
if (PKG_CONFIG_FOUND)
  execute_process(COMMAND ${PKG_CONFIG_EXECUTABLE} --variable=pkgdatadir wayland-protocols
    OUTPUT_VARIABLE WAYLANDPROTOCOLS_PATH
    OUTPUT_STRIP_TRAILING_WHITESPACE)
endif ()
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(WaylandProtocols DEFAULT_MSG WAYLANDPROTOCOLS_PATH)
mark_as_advanced(wAYLANDPROTOCOLS_PATH)
set(XDG_SHELL_PROTOCOL     ${WAYLANDPROTOCOLS_PATH}/stable/xdg-shell/xdg-shell.xml)
set(ZWP_TABLET_V2_PROTOCOL ${WAYLANDPROTOCOLS_PATH}/unstable/tablet/tablet-unstable-v2.xml)
set(ZWP_LINUX_DMABUF_V1_PROTOCOL    ${WAYLANDPROTOCOLS_PATH}/stable/linux-dmabuf/linux-dmabuf-v1.xml)
message(XDG_SHELL_PROTOCOL="${XDG_SHELL_PROTOCOL}")
message(ZWP_TABLET_V2_PROTOCOL="${ZWP_TABLET_V2_PROTOCOL}")
message(ZWP_LINUX_DMA_BUF_V1="${ZWP_LINUX_DMABUF_V1_PROTOCOL}")
add_custom_command(
  OUTPUT xdg-shell-private.c
  COMMAND wayland-scanner client-header ${XDG_SHELL_PROTOCOL} xdg-shell-client.h
  COMMAND wayland-scanner private-code  ${XDG_SHELL_PROTOCOL} xdg-shell-private.c)
add_custom_command(
  OUTPUT zwp-tablet-v2-private.c
  COMMAND wayland-scanner client-header ${ZWP_TABLET_V2_PROTOCOL} zwp-tablet-v2-client.h
  COMMAND wayland-scanner private-code  ${ZWP_TABLET_V2_PROTOCOL} zwp-tablet-v2-private.c)
add_custom_command(
  OUTPUT zwp-linux-dmabuf-v1-private.c
  COMMAND wayland-scanner client-header ${ZWP_LINUX_DMABUF_V1_PROTOCOL} zwp-linux-dmabuf-v1-client.h
  COMMAND wayland-scanner private-code  ${ZWP_LINUX_DMABUF_V1_PROTOCOL} zwp-linux-dmabuf-v1-private.c)

# build steps
project(criss-cross)

add_executable(criss-cross
  main.cc
  ${CMAKE_CURRENT_BINARY_DIR}/xdg-shell-private.c
  ${CMAKE_CURRENT_BINARY_DIR}/zwp-tablet-v2-private.c
  ${CMAKE_CURRENT_BINARY_DIR}/zwp-linux-dmabuf-v1-private.c)

target_link_libraries(criss-cross
  wayland-client)

# gtest
file(GLOB_RECURSE TST *-test.cc)
list(FILTER TST EXCLUDE REGEX "build/.*")
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG v1.17.0
)
FetchContent_MakeAvailable(googletest)
enable_testing()
add_executable(criss-cross-test ${TST})
target_link_libraries(criss-cross-test PRIVATE gtest_main)
include(GoogleTest)
gtest_discover_tests(criss-cross-test)

add_custom_target(run
  DEPENDS criss-cross criss-cross-test
  COMMAND ctest && WAYLAND_DEBUG=1 ./criss-cross)
