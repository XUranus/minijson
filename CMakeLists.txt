CMAKE_MINIMUM_REQUIRED(VERSION 3.14)
SET(Project minijson)
PROJECT(${Project} C CXX)

SET(SOURCE_DIR ${${Project}_SOURCE_DIR})
SET(CMAKE_C_STANDARD 99)
SET(CMAKE_CXX_STANDARD 11)
SET(CMAKE_POSITION_INDEPENDENT_CODE on)
SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/")

MESSAGE("Compiling minijson")
MESSAGE(STATUS "Source directory is " ${SOURCE_DIR})
MESSAGE("Project = ${Project}")
MESSAGE("CMAKE_BUILD_TYPE = ${CMAKE_BUILD_TYPE}")
MESSAGE("COVERAGE = ${COVERAGE}")

# supress MSVC/GCC warnings
IF(${CMAKE_HOST_WIN32})
    SET(CMAKE_CXX_FLAGS_DEBUG "/MTd /Zi /Ob0 /Od /RTC1")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4251")
ENDIF()

# define headers and sources
SET(Headers Json.h)
SET(Sources Json.cpp)

# build a dynamic library
MESSAGE("Build dynamic library ${MINIJSON_DYNAMIC_LIBRARY_TARGET}")
SET(MINIJSON_DYNAMIC_LIBRARY_TARGET ${Project})
ADD_LIBRARY(${MINIJSON_DYNAMIC_LIBRARY_TARGET} SHARED ${Sources} ${Headers})
# to generate export library when build dynamic library
TARGET_COMPILE_DEFINITIONS(${MINIJSON_DYNAMIC_LIBRARY_TARGET} PRIVATE -DLIBRARY_EXPORT)

# build a static library
MESSAGE("Build static library ${MINIJSON_STATIC_LIBRARY_TARGET}")
SET(MINIJSON_STATIC_LIBRARY_TARGET ${Project}_static)
ADD_LIBRARY(${MINIJSON_STATIC_LIBRARY_TARGET}  STATIC ${Sources} ${Headers})

# set -DCMAKE_BUILD_TYPE=Debug to enable LLT, set -DCOVERAGE=ON to enable code coverage
IF (CMAKE_BUILD_TYPE STREQUAL "Debug")
    # these config must be put at the level of source code in order to append compile flags
    IF (${COVERAGE} STREQUAL "lcov" OR ${COVERAGE} STREQUAL "gcovr")
        MESSAGE("code coverage is enabled, COVERAGE = ${COVERAGE}, append coverage compiler flags")
        INCLUDE(${SOURCE_DIR}/test/CodeCoverage.cmake)
        APPEND_COVERAGE_COMPILER_FLAGS()
    ENDIF()

    ENABLE_TESTING()
    ADD_SUBDIRECTORY("test")
ENDIF()