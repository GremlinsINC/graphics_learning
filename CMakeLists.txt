cmake_minimum_required(VERSION 3.10)
project(vulkan-playground)

set(CMAKE_CXX_STANDARD 20)

find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED)

add_executable(vulkan-playground src/main.cpp)
target_link_libraries(vulkan-playground Vulkan::Vulkan glfw)

