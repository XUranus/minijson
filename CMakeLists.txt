cmake_minimum_required(VERSION 3.14)

set(Project jsoncpp)
project(${Project} C CXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_POSITION_INDEPENDENT_CODE on)

enable_testing()

SET(Headers
  Json.h
)

SET(Sources
  Json.cpp
)

add_library(${Project} STATIC ${Sources} ${Headers})

add_subdirectory(googletest)
add_subdirectory(test)