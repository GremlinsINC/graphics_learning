cmake_minimum_required(VERSION 3.10)
project(MyGraphicApp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)

pkg_check_modules(GLFW REQUIRED glfw3)
pkg_check_modules(Vulkan REQUIRED vulkan)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/src/shaders DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_executable(MyGraphicApp
    src/main.cpp
    src/Window.cpp
    src/VulkanContext.cpp
    src/Swapchain.cpp
    src/CommandBuffer.cpp
    src/Triangle.cpp
    src/TriangleRenderer.cpp
    src/math/Vector2.cpp
    src/math/Matrix3.cpp
)

target_include_directories(MyGraphicApp PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${GLFW_INCLUDE_DIRS}
    ${Vulkan_INCLUDE_DIRS}
)

target_link_libraries(MyGraphicApp 
    ${GLFW_LIBRARIES}
    ${Vulkan_LIBRARIES}
)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(VulkanApp PRIVATE 
        -Wall -Wextra -Wpedantic -g
        -O2
    )
endif()
