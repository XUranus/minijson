cmake_minimum_required(VERSION 3.14)
set(Project minijson)
project(${Project} C CXX)

set(SOURCE_DIR ${${Project}_SOURCE_DIR})
set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_POSITION_INDEPENDENT_CODE on)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/")

message("Compiling minijson")
message(STATUS "Source directory is " ${SOURCE_DIR})
message("Project = ${Project}")
message("CMAKE_BUILD_TYPE = ${CMAKE_BUILD_TYPE}")
message("COVERAGE = ${COVERAGE}")

# supress MSVC/GCC warnings
if(${CMAKE_HOST_WIN32})
    set(CMAKE_CXX_FLAGS_DEBUG "/MTd /Zi /Ob0 /Od /RTC1")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4251")
endif()

# define headers and sources
set(Headers Json.h)
set(Sources Json.cpp)

# build a dynamic library
message("Build dynamic library ${MINIJSON_DYNAMIC_LIBRARY_TARGET}")
set(MINIJSON_DYNAMIC_LIBRARY_TARGET ${Project})
add_library(${MINIJSON_DYNAMIC_LIBRARY_TARGET} SHARED ${Sources} ${Headers})
# to generate export library when build dynamic library
target_compile_definitions(${MINIJSON_DYNAMIC_LIBRARY_TARGET} PRIVATE -DLIBRARY_EXPORT)

# build a static library
message("Build static library ${MINIJSON_STATIC_LIBRARY_TARGET}")
set(MINIJSON_STATIC_LIBRARY_TARGET ${Project}_static)
add_library(${MINIJSON_STATIC_LIBRARY_TARGET}  STATIC ${Sources} ${Headers})

# set -DCMAKE_BUILD_TYPE=Debug to enable LLT, set -DCOVERAGE=ON to enable code coverage
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    # these config must be put at the level of source code in order to append compile flags
    if (${COVERAGE} STREQUAL "lcov" OR ${COVERAGE} STREQUAL "gcovr")
        message("code coverage is enabled, COVERAGE = ${COVERAGE}, append coverage compiler flags")
        INCLUDE(${SOURCE_DIR}/test/CodeCoverage.cmake)
        APPEND_COVERAGE_COMPILER_FLAGS()
    endif()

    enable_testing()
    add_subdirectory("test")
endif()