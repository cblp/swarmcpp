cmake_minimum_required(VERSION 3.5)
project(swarmcpp)

INCLUDE_DIRECTORIES(/home/gritzko/Projects/rocksdb/include)
LINK_DIRECTORIES(/home/gritzko/Projects/rocksdb/)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

add_executable(test64x64 test/64x64.cpp)

set(SOURCE_FILES main.cpp test/64x64.cpp)

# add_executable(swarmcpp ${SOURCE_FILES})

# TARGET_LINK_LIBRARIES(swarmcpp rocksdb)