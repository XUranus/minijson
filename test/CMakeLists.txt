cmake_minimum_required(VERSION 3.14)

SET(Project minijson_test)
project(${Project} C CXX)

SET(Headers
    StructSample.h
)

SET(Sources
    MiniJsonTest.cpp
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# auto fetch googletest
include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(${Project} ${Sources} ${Headers})

target_link_libraries(${Project} PUBLIC 
        minijson_static
        GTest::gtest_main
)

add_test(
        NAME ${Project}
        COMMAND ${Project}
)

include(GoogleTest)
gtest_discover_tests(${Project})