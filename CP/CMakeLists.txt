cmake_minimum_required(VERSION 3.10)
project(DagSchedulerSimple LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(scheduler
        src/scheduler.cpp
        src/graph.cpp
        src/message.cpp
)
target_link_libraries(scheduler PRIVATE zmq)

add_executable(job
        src/job.cpp
        src/message.cpp
)
target_link_libraries(job PRIVATE zmq)
