cmake_minimum_required(VERSION 3.5)
project(swarmcpp)

INCLUDE_DIRECTORIES(/home/gritzko/Projects/rocksdb/include)
LINK_DIRECTORIES(/home/gritzko/Projects/rocksdb/)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

add_executable(test64x64 test/64x64.cpp include/64x64.h)
add_executable(testuid test/uid.cpp include/uid.h)
add_executable(testspec test/spec.cpp include/spec.h)
add_executable(testop test/op.cpp include/op.h include/frame.h include/slice.h include/value.h)

set(SOURCE_FILES main.cpp test/64x64.cpp test/uid.cpp test/spec.cpp test/op.cpp)

# add_executable(swarmcpp ${SOURCE_FILES})

# TARGET_LINK_LIBRARIES(swarmcpp rocksdb)
