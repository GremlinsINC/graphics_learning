#include "Window.hpp"
#include "VulkanContext.hpp"
#include "Swapchain.hpp"
#include "CommandBuffer.hpp"
#include <stdexcept>
#include <iostream>
#include <array>

class VulkanClearApp {
public:
    void run() {
        try {
            std::cout << "Initializing Vulkan application..." << std::endl;
            init();
            std::cout << "Starting main loop..." << std::endl;
            mainLoop();
            std::cout << "Cleaning up..." << std::endl;
            cleanup();
        } catch (const std::exception& e) {
            std::cerr << "Fatal error: " << e.what() << std::endl;
            throw;
        }
    }

private:
    Window window_{800, 600, "Vulkan Clear App"};
    std::unique_ptr<VulkanContext> context_;
    std::unique_ptr<Swapchain> swapchain_;
    std::unique_ptr<CommandBuffer> commandBuffer_;
    
    VkSemaphore imageAvailableSemaphore_ = VK_NULL_HANDLE;
    VkSemaphore renderFinishedSemaphore_ = VK_NULL_HANDLE;
    
    void init() {
        std::cout << "Creating Vulkan context..." << std::endl;
        context_ = std::make_unique<VulkanContext>(window_);
        
        std::cout << "Creating swapchain..." << std::endl;
        swapchain_ = std::make_unique<Swapchain>(*context_);
        
        std::cout << "Creating command buffer..." << std::endl;
        commandBuffer_ = std::make_unique<CommandBuffer>(*context_, *swapchain_);
        
        std::cout << "Creating semaphores..." << std::endl;
        createSemaphores();
        
        std::cout << "Initialization completed successfully" << std::endl;
    }
    
    void createSemaphores() {
        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        
        VkResult result1 = vkCreateSemaphore(context_->getDevice(), &semaphoreInfo, nullptr, &imageAvailableSemaphore_);
        VkResult result2 = vkCreateSemaphore(context_->getDevice(), &semaphoreInfo, nullptr, &renderFinishedSemaphore_);
        
        if (result1 != VK_SUCCESS || result2 != VK_SUCCESS) {
            throw std::runtime_error("Failed to create semaphores");
        }
    }
    
    void mainLoop() {
        std::cout << "Entering main loop..." << std::endl;
        while (!window_.shouldClose()) {
            window_.pollEvents();
            drawFrame();
        }
        
        vkDeviceWaitIdle(context_->getDevice());
    }
    
    void drawFrame() {
        uint32_t imageIndex;
        
        // Получаем следующее изображение из swapchain (теперь с правильным возвращаемым типом)
        VkResult acquireResult = swapchain_->acquireNextImage(imageAvailableSemaphore_, &imageIndex);
        if (acquireResult != VK_SUCCESS && acquireResult != VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error("Failed to acquire swap chain image");
        }
        
        // Очищаем изображение
        std::array<float, 4> clearColor = {1.0f, 1.0f, 1.0f, 1.0f};
        commandBuffer_->recordClearCommand(imageIndex, clearColor);
        
        // Отправляем команды в очередь
        commandBuffer_->submit(imageIndex, imageAvailableSemaphore_, renderFinishedSemaphore_);
        
        // Презентуем изображение
        swapchain_->present(imageIndex, renderFinishedSemaphore_);
        
        // Ждем завершения операций для этого кадра
        vkQueueWaitIdle(context_->getGraphicsQueue());
    }
    
    void cleanup() {
        if (context_) {
            vkDeviceWaitIdle(context_->getDevice());
            
            if (imageAvailableSemaphore_ != VK_NULL_HANDLE) {
                vkDestroySemaphore(context_->getDevice(), imageAvailableSemaphore_, nullptr);
            }
            if (renderFinishedSemaphore_ != VK_NULL_HANDLE) {
                vkDestroySemaphore(context_->getDevice(), renderFinishedSemaphore_, nullptr);
            }
        }
    }
};

int main() {
    VulkanClearApp app;
    
    try {
        app.run();
        std::cout << "Application finished successfully" << std::endl;
        return EXIT_SUCCESS;
    } catch (const std::exception& e) {
        std::cerr << "Application error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}
