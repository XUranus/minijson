cmake_minimum_required(VERSION 3.14)

set(Project minijson)
project(${Project} C CXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_POSITION_INDEPENDENT_CODE on)
set(CMAKE_CXX_FLAGS_DEBUG "/MTd /Zi /Ob0 /Od /RTC1")

# code coverage
IF (CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "Coverage")
  FIND_PROGRAM(LCOV_PATH lcov)
  FIND_PROGRAM(GENHTML_PATH genhtml)

  MESSAGE( INFO "check lcov and genhtml for code coverage" )
  IF(NOT LCOV_PATH)
    MESSAGE(FATAL_ERROR "lcov not found!")
  ENDIF()

  IF(NOT GENHTML_PATH)
    MESSAGE(FATAL_ERROR "genhtml not found!")
  ENDIF()

  SET(GCC_COVERAGE_COMPILE_FLAGS "-g -O0 -coverage -fprofile-arcs -ftest-coverage")
  SET(GCC_COVERAGE_LINK_FLAGS "-coverage -lgcov")
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}")
  SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${GCC_COVERAGE_LINK_FLAGS}")
ENDIF()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

enable_testing()

SET(Headers
  Json.h
)

SET(Sources
  Json.cpp
)

add_library(${Project} SHARED ${Sources} ${Headers})
add_library(${Project}_static STATIC ${Sources} ${Headers})

add_subdirectory(test)