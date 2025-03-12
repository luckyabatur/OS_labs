cmake_minimum_required(VERSION 3.10)
project(distributed_computing)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)


add_library(common_lib
        src/nodes.cpp
        src/common.cpp
)

target_include_directories(common_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(control src/control_node.cpp)
add_executable(computing src/computing_node.cpp)


target_link_libraries(control
        PRIVATE
        common_lib
        zmq
)

target_link_libraries(computing
        PRIVATE
        common_lib
        zmq
)


