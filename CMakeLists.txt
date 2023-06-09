cmake_minimum_required(VERSION 3.24)
project(dsview)

option(INSOURCEBUILD OFF) # Prevent in-source style builds
if((${PROJECT_SOURCE_DIR} STREQUAL ${PROJECT_BINARY_DIR}) AND NOT
                                                              ${INSOURCEBUILD})
    message(
        FATAL_ERROR
            "In-source building disabled. Provide -DINSOURCEBUILD option if you are sure about it."
    )
endif()

option(SANITIZE OFF)
if(${SANITIZE})
    add_compile_options(-fsanitize=undefined,address -fno-omit-frame-pointer)
    add_link_options(-fsanitize=undefined,address -fno-omit-frame-pointer)
endif()

option(THREAD_SANITIZE OFF)
if(${THREAD_SANITIZE})
    add_compile_options(-fsanitize=thread -fno-omit-frame-pointer)
    add_link_options(-fsanitize=thread -fno-omit-frame-pointer)
endif()

if(THREAD_SANITIZE AND SANITIZE)
    message(FATAL_ERROR "Thread and address sanitizer can't be used together")
endif()

include(cmake/functions.cmake)
include(cmake/dependencies.cmake)

set(DSLIB_SOURCES lib/device.cc)

add_library(dslib ${DSLIB_SOURCES})
target_link_libraries(dslib PUBLIC Boost::boost fixed_string fmt)
target_compile_features(dslib PUBLIC cxx_std_20)
target_include_directories(dslib PUBLIC include)
enable_warnings(dslib)
target_enable_linter(dslib)

set(DSVIEW_SOURCES src/main.cc)

option(DSVIEW_NO_APP OFF)
if(NOT DSVIEW_NO_APP)
    add_executable(dsview ${DSVIEW_SOURCES})
    target_link_libraries(dsview PRIVATE Boost::boost dslib)
    target_compile_features(dsview PUBLIC cxx_std_20)
    enable_warnings(dsview)
    target_enable_linter(dsview)
endif()

option(DSVIEW_BUILD_DOCS OFF)
if(DSVIEW_BUILD_DOCS)
    find_package(Doxygen REQUIRED OPTIONAL_COMPONENTS mscgen dia)
    doxygen_add_docs(${CMAKE_PROJECT}_docs include ALL)
endif()

option(DSVIEW_BUILD_TESTS OFF)
if(DSVIEW_BUILD_TESTS)
    enable_testing()
    add_subdirectory(test)
endif()
