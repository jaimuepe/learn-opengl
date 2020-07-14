cmake_minimum_required(VERSION 3.6)

project(learn-opengl)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(glfw)
add_subdirectory(glad)

include_directories(glfw/include)
include_directories(glad/include)

link_libraries(glfw glad)

add_subdirectory("1. Getting started")
