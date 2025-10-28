#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <string>

class Window {
public:
    Window(int width, int height, const std::string& title);
    ~Window();
    
    // Delete copy operations
    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;
    
    // Allow move operations
    Window(Window&&) = default;
    Window& operator=(Window&&) = default;
    
    bool shouldClose() const { return glfwWindowShouldClose(window_); }
    void pollEvents() const { glfwPollEvents(); }
    GLFWwindow* getHandle() const { return window_; }
    
    void createSurface(VkInstance instance, VkSurfaceKHR* surface);
    std::pair<int, int> getFramebufferSize() const;
    
private:
    GLFWwindow* window_;
    int width_, height_;
    std::string title_;
    
    static void framebufferResizeCallback(GLFWwindow* window, int width, int height);
};
