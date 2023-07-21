CMAKE_MINIMUM_REQUIRED(VERSION 3.14)
SET(Project "minijson_test")
SET(${Project} C CXX)

SET(Headers StructSample.h)
SET(Sources MiniJsonTest.cpp)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# build test and run code coverage
ENABLE_TESTING()

# auto fetch googletest
INCLUDE(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
SET(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

ADD_EXECUTABLE(${Project} ${Sources} ${Headers})

TARGET_LINK_LIBRARIES(${Project} PUBLIC 
    minijson_static
    GTest::gtest_main
)

ADD_TEST(
    NAME ${Project}
    COMMAND ${Project}
)

INCLUDE(GoogleTest)
gtest_discover_tests(${Project})

# configure code coverage
IF (${COVERAGE} STREQUAL "lcov")
    MESSAGE("code coverage is enabled, using lcov")
    INCLUDE(CodeCoverage.cmake)
    APPEND_COVERAGE_COMPILER_FLAGS()
    SETUP_TARGET_FOR_COVERAGE_LCOV(
        NAME "minijson_coverage_test"
        EXECUTABLE make test
        EXCLUDE "${PROJECT_SOURCE_DIR}/test/*" "/usr/include/*" "*googlemock*" "*googletest*"
    )
ENDIF()

IF (${COVERAGE} STREQUAL "gcovr")
    MESSAGE("code coverage is enabled, using gcovr")
    INCLUDE(CodeCoverage.cmake)
    APPEND_COVERAGE_COMPILER_FLAGS()
    SETUP_TARGET_FOR_COVERAGE_GCOVR_HTML(
        NAME "minijson_coverage_test"
        EXECUTABLE make test
        BASE_DIRECTORY "${PROJECT_SOURCE_DIR}"
        EXCLUDE "${PROJECT_SOURCE_DIR}/test/*" "/usr/include/*" "*googlemock*" "*googletest*" "*build*"
    )
ENDIF()