CMAKE_MINIMUM_REQUIRED(VERSION 3.14)

SET(Project minijson)
PROJECT(${Project} C CXX)
SET(SOURCE_DIR ${${Project}_SOURCE_DIR})

MESSAGE(STATUS "source directory is "${SOURCE_DIR})

SET(CMAKE_C_STANDARD 99)
SET(CMAKE_CXX_STANDARD 11)
SET(CMAKE_POSITION_INDEPENDENT_CODE on)

IF(${CMAKE_HOST_WIN32})
    SET(CMAKE_CXX_FLAGS_DEBUG "/MTd /Zi /Ob0 /Od /RTC1")
endIF()

SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/")
SET(Headers Json.h)
SET(Sources Json.cpp)

add_library(${Project} SHARED ${Sources} ${Headers})
add_library(${Project}_static STATIC ${Sources} ${Headers})

# build test and run code coverage
IF (CMAKE_BUILD_TYPE STREQUAL "Coverage")

    ENABLE_TESTING()

    INCLUDE(${SOURCE_DIR}/CodeCoverage.cmake)
    APPEND_COVERAGE_COMPILER_FLAGS()
    SETUP_TARGET_FOR_COVERAGE_LCOV(
        NAME minijson_lcov_test
        EXECUTABLE make test
        EXCLUDE "${PROJECT_SOURCE_DIR}/test/*" "/usr/include/*" "*googlemock*" "*googletest*"
    )
    
    SETUP_TARGET_FOR_COVERAGE_GCOVR_HTML(
        NAME minijson_gcovr_test
        EXECUTABLE make test
        BASE_DIRECTORY "${PROJECT_SOURCE_DIR}"
        EXCLUDE "${PROJECT_SOURCE_DIR}/test/*" "/usr/include/*" "*googlemock*" "*googletest*" "*build*"
    )

    ADD_SUBDIRECTORY("test")
ENDIF()