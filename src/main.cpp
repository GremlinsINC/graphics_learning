#include <GLFW/glfw3.h>
#include <iostream>
#include <thread>
#include <chrono>

int main() {
    // Установка Wayland платформы ДО инициализации
    glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_WAYLAND);
    
    if (!glfwInit()) {
        std::cerr << "GLFW init failed\n";
        return -1;
    }

    // Настройка для Vulkan
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // Важно для Vulkan
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);

    GLFWwindow* win = glfwCreateWindow(800, 600, "Vulkan Wayland Window", nullptr, nullptr);
    if (!win) {
        std::cerr << "GLFW create window failed\n";
        glfwTerminate();
        return -1;
    }

    // Явно показываем окно
    glfwShowWindow(win);
    
    // Принудительная обработка событий
    glfwPollEvents();
    
    // Даем время Wayland обработать запрос на отображение
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    std::cout << "Window created successfully!" << std::endl;
    std::cout << "Window visible: " << glfwGetWindowAttrib(win, GLFW_VISIBLE) << std::endl;
    std::cout << "Window focused: " << glfwGetWindowAttrib(win, GLFW_FOCUSED) << std::endl;

    // Основной цикл с более агрессивной обработкой событий
    while (!glfwWindowShouldClose(win)) {
        // Очистка/рендеринг Vulkan должен быть здесь
        
        glfwPollEvents();
        
        // Небольшая задержка для снижения нагрузки на CPU
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }

    glfwDestroyWindow(win);
    glfwTerminate();
    return 0;
}
