cmake_minimum_required(VERSION 3.14)

set(Project minijson)
project(${Project} C CXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_POSITION_INDEPENDENT_CODE on)
set(CMAKE_CXX_FLAGS_DEBUG "/MTd /Zi /Ob0 /Od /RTC1")

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