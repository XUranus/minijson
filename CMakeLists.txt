cmake_minimum_required(VERSION 3.14)

set(Project minijson)
project(${Project} C CXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_POSITION_INDEPENDENT_CODE on)
set(CMAKE_CXX_FLAGS_DEBUG "/MTd /Zi /Ob0 /Od /RTC1")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/")
# code coverage
IF (CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "Coverage")
  include(CodeCoverage)
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
ENDIF()

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