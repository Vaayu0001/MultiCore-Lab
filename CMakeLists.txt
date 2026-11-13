cmake_minimum_required(VERSION 3.20)

project(MultiCoreLab)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)

# Thread Basics
add_executable(thread_test src/main.cpp)
target_link_libraries(thread_test PRIVATE Threads::Threads)

add_executable(thread_basics src/thread_basics.cpp)
target_link_libraries(thread_basics PRIVATE Threads::Threads)

# Synchronization
add_executable(mutex_basics src/mutex_basics.cpp)
target_link_libraries(mutex_basics PRIVATE Threads::Threads)

# Data Parallelism
add_executable(basic_data_parallelism
    src/data_parallelism/basic_data_parallelism.cpp
)
target_link_libraries(basic_data_parallelism PRIVATE Threads::Threads)

add_executable(data_parallelism_benchmark
    src/data_parallelism/data_parallelism_benchmark.cpp
)
target_link_libraries(data_parallelism_benchmark PRIVATE Threads::Threads)

# Task Parallelism
add_executable(basic_task_parallelism
    src/task_parallelism/basic_task_parallelism.cpp
)
target_link_libraries(basic_task_parallelism PRIVATE Threads::Threads)

# Thread Pool
add_executable(thread_pool
    src/task_parallelism/thread_pool.cpp
)
target_link_libraries(thread_pool PRIVATE Threads::Threads)

# Task Parallelism Benchmark
add_executable(task_parallelism_benchmark
    src/task_parallelism/task_parallelism_benchmark.cpp
)
target_link_libraries(task_parallelism_benchmark PRIVATE Threads::Threads)

# Central Application
add_executable(multicore_lab src/main_menu.cpp)
