include(FetchContent)
set(FETCHCONTENT_QUIET OFF)

FetchContent_Declare(
    fixed_string_lib
    GIT_REPOSITORY https://github.com/unterumarmung/fixed_string.git
    GIT_TAG v0.1.1)

FetchContent_Declare(
    fmt_lib
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG 9.0.0
    FIND_PACKAGE_ARGS
    9.0.0
    NAMES
    fmt)

FetchContent_Declare(
    ranges_lib
    GIT_REPOSITORY https://github.com/ericniebler/range-v3
    GIT_TAG 0.12.0
    FIND_PACKAGE_ARGS
    0.12.0
    NAMES
    range-v3)

FetchContent_MakeAvailable(fixed_string_lib fmt_lib ranges_lib)

find_package(Boost 1.81 REQUIRED COMPONENTS ALL)

if(DSVIEW_BUILD_TESTS)

    FetchContent_Declare(
        gtest_lib
        GIT_REPOSITORY https://github.com/google/googletest
        GIT_TAG v1.13.0
        FIND_PACKAGE_ARGS
        1.12.0
        NAMES
        GTest)

    FetchContent_MakeAvailable(gtest_lib)

endif()
