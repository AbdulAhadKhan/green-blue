cmake_minimum_required(VERSION 3.16.3)

project("green-blue" VERSION 0.0.0)
file(GLOB_RECURSE SOURCES "utils/*.c")

set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)

add_executable(${PROJECT_NAME} "green-blue.c" ${SOURCES})
# add_executable("server-blue" "server-blue.c" ${SOURCES})
add_executable("captain-balancer" "captain-balancer.c" ${SOURCES})

target_link_libraries("captain-balancer" Threads::Threads)