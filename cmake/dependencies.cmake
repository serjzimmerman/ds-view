include(FetchContent)
set(FETCHCONTENT_QUIET OFF)

FetchContent_Declare(
    fixed_string_lib
    GIT_REPOSITORY https://github.com/unterumarmung/fixed_string.git
    GIT_TAG v0.1.1)

FetchContent_Declare(
    fmt_lib
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG 10.0.0
    FIND_PACKAGE_ARGS NAMES fmt)

FetchContent_MakeAvailable(fixed_string_lib fmt_lib)

find_package(Boost 1.81 REQUIRED)

if (DSVIEW_BUILD_TESTS)

FetchContent_Declare(
    gtest_lib
    GIT_REPOSITORY https://github.com/google/googletest
    GIT_TAG v1.13.0
    FIND_PACKAGE_ARGS NAMES gtest)

FetchContent_MakeAvailable(gtest_lib)

endif()