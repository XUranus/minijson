cmake_minimum_required(VERSION 3.14)

SET(Project jsoncpp_test)
project(${Project} C CXX)

SET(Sources
  JsonTest.cpp
)

add_executable(${Project} ${Sources})

target_link_libraries(${Project} PUBLIC 
    gtest_main
    jsoncpp
)

add_test(
    NAME ${Project}
    COMMAND ${Project}
)