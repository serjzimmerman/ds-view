include(GoogleTest)

set(DSLIB_TEST_SOURCES
    src/idn.cc)

add_executable(dslib_test ${DSLIB_TEST_SOURCES})
gtest_add_tests(TARGET dslib_test ${DSLIB_TEST_SOURCES})
target_link_libraries(dslib_test dslib gtest gtest_main fmt range-v3)