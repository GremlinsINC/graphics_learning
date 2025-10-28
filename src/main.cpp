#include "Window.hpp"
#include "VulkanContext.hpp"
#include "Swapchain.hpp"
#include "CommandBuffer.hpp"
#include "Triangle.hpp"
#include "TriangleRenderer.hpp"
#include <stdexcept>
#include <iostream>
#include <array>
#include <memory>

class VulkanTriangleApp {
public:
    void run() {
        try {
            std::cout << "Initializing Vulkan application..." << std::endl;
            init();
            createTriangles();
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
    Window window_{800, 800, "Vulkan Triangle App"};
    std::unique_ptr<VulkanContext> context_;
    std::unique_ptr<Swapchain> swapchain_;
    std::unique_ptr<CommandBuffer> commandBuffer_;
    std::unique_ptr<TriangleRenderer> triangleRenderer_;
    
    VkSemaphore imageAvailableSemaphore_ = VK_NULL_HANDLE;
    VkSemaphore renderFinishedSemaphore_ = VK_NULL_HANDLE;
    
    void init() {
        std::cout << "Creating Vulkan context..." << std::endl;
        context_ = std::make_unique<VulkanContext>(window_);
        
        std::cout << "Creating swapchain..." << std::endl;
        swapchain_ = std::make_unique<Swapchain>(*context_);
        
        std::cout << "Creating triangle renderer..." << std::endl;
        triangleRenderer_ = std::make_unique<TriangleRenderer>(*context_, *swapchain_);
        
        std::cout << "Creating command buffer..." << std::endl;
        commandBuffer_ = std::make_unique<CommandBuffer>(*context_, *swapchain_);
        
        std::cout << "Creating semaphores..." << std::endl;
        createSemaphores();
        
        std::cout << "Initialization completed successfully" << std::endl;
    }
    
    void createTriangles() {
        
        auto triangle1 = std::make_unique<Triangle>(
            Vector2(1.0f, 0.5f),
            Vector2(1.0f, -0.5f),
            Vector2(0.0f, 0.5f)
        );

        auto triangle2 = std::make_unique<Triangle>(
            Vector2(-1.0f, 0.5f),
            Vector2(-1.0f, -0.5f),
            Vector2(0.0f, 0.5f)
        );
        auto triangle3 = std::make_unique<Triangle>(
            Vector2(3.0f, 0.0f),
            Vector2(1.0f, 0.5f),
            Vector2(1.0f, -0.5f)
        );
        auto triangle4 = std::make_unique<Triangle>(
            Vector2(-3.0f, 0.0f),
            Vector2(-1.0f, 0.5f),
            Vector2(-1.0f, -0.5f)
        );
        auto triangle5 = std::make_unique<Triangle>(
            Vector2(1.0f, -0.5f),
            Vector2(-1.0f, -0.5f),
            Vector2(0.0f, 0.5f)
        );

        auto triangle6 = std::make_unique<Triangle>(
            Vector2(1.0f, -0.5f),
            Vector2(-1.0f, -0.5f),
            Vector2(0.0f, -3.5f)
        );
        auto triangle7 = std::make_unique<Triangle>(
            Vector2(-0.5f, -4.0f),
            Vector2(0.0f, -3.5f),
            Vector2(0.0f, -4.5f)
        );
        auto triangle8 = std::make_unique<Triangle>(
            Vector2(0.5f, -4.0f),
            Vector2(0.0f, -3.5f),
            Vector2(0.0f, -4.5f)
        );
        auto triangle9 = std::make_unique<Triangle>(
            Vector2(1.0f, 0.5f),
            Vector2(-1.0f, 0.5f),
            Vector2(0.0f, 1.5f)
        );
        auto triangle10 = std::make_unique<Triangle>(
            Vector2(-1.0f, 2.5f),
            Vector2(-1.0f, 0.5f),
            Vector2(0.0f, 1.5f)
        );

        auto triangle11 = std::make_unique<Triangle>(
            Vector2(1.0f, 2.5f),
            Vector2(1.0f, 0.5f),
            Vector2(0.0f, 1.5f)
        );
        auto triangle12 = std::make_unique<Triangle>(
            Vector2(1.0f, 2.5f),
            Vector2(-1.0f, 2.5f),
            Vector2(0.0f, 1.5f)
        );
        auto triangle13 = std::make_unique<Triangle>(
            Vector2(1.0f, 2.5f),
            Vector2(-1.0f, 2.5f),
            Vector2(0.0f, 3.5f)
        );
        auto triangle14 = std::make_unique<Triangle>(
            Vector2(1.0f, 4.5f),
            Vector2(1.0f, 2.5f),
            Vector2(0.0f, 3.5f)
        );

        auto triangle15 = std::make_unique<Triangle>(
            Vector2(-1.0f, 4.5f),
            Vector2(-1.0f, 2.5f),
            Vector2(0.0f, 3.5f)
        );
        auto triangle16 = std::make_unique<Triangle>(
            Vector2(1.0f, 4.5f),
            Vector2(-1.0f, 4.5f),
            Vector2(0.0f, 5.5f)
        );
        auto triangle17 = std::make_unique<Triangle>(
            Vector2(0.5f, -4.0f),
            Vector2(0.0f, -3.5f),
            Vector2(0.0f, -4.5f)
        );



        triangle1->setWorldPosition(Vector2(0.0f, 0.0f));
        triangle1->setColor(1.0f, 0.0f, 0.0f); // Красный
        triangle2->setWorldPosition(Vector2(0.0f, 0.0f));
        triangle2->setColor(1.0f, 0.0f, 0.0f); // Красный
        triangle3->setWorldPosition(Vector2(0.0f, 0.0f));
        triangle3->setColor(1.0f, 0.0f, 0.0f); // Красный
        triangle4->setWorldPosition(Vector2(0.0f, 0.0f));
        triangle4->setColor(1.0f, 0.0f, 0.0f); // Красный
        triangle5->setWorldPosition(Vector2(0.0f, 0.0f));
        triangle5->setColor(1.0f, 0.0f, 0.0f); // Красный
        triangle6->setWorldPosition(Vector2(0.0f, 0.0f));
        triangle6->setColor(1.0f, 0.0f, 0.0f); // Красный
        triangle7->setWorldPosition(Vector2(0.0f, 0.0f));
        triangle7->setColor(1.0f, 0.0f, 0.0f); // Красный
        triangle8->setWorldPosition(Vector2(0.0f, 0.0f));
        triangle8->setColor(1.0f, 0.0f, 0.0f); // Красный
        triangle9->setWorldPosition(Vector2(0.0f, 0.0f));
        triangle9->setColor(1.0f, 0.0f, 0.0f); // Красный
        triangle10->setWorldPosition(Vector2(0.0f, 0.0f));
        triangle10->setColor(1.0f, 0.0f, 0.0f); // Красный
        triangle11->setWorldPosition(Vector2(0.0f, 0.0f));
        triangle11->setColor(1.0f, 0.0f, 0.0f); // Красный
        triangle12->setWorldPosition(Vector2(0.0f, 0.0f));
        triangle12->setColor(1.0f, 0.0f, 0.0f); // Красный
        triangle13->setWorldPosition(Vector2(0.0f, 0.0f));
        triangle13->setColor(1.0f, 0.0f, 0.0f); // Красный
        triangle14->setWorldPosition(Vector2(0.0f, 0.0f));
        triangle14->setColor(1.0f, 0.0f, 0.0f); // Красный
        triangle15->setWorldPosition(Vector2(0.0f, 0.0f));
        triangle15->setColor(1.0f, 0.0f, 0.0f); // Красный
        triangle16->setWorldPosition(Vector2(0.0f, 0.0f));
        triangle16->setColor(1.0f, 0.0f, 0.0f); // Красный
        triangle17->setWorldPosition(Vector2(0.0f, 0.0f));
        triangle17->setColor(1.0f, 0.0f, 0.0f); // Красный

        triangleRenderer_->addTriangle(std::move(triangle1));
        triangleRenderer_->addTriangle(std::move(triangle2));
        triangleRenderer_->addTriangle(std::move(triangle3));
        triangleRenderer_->addTriangle(std::move(triangle4));
        triangleRenderer_->addTriangle(std::move(triangle5));
        triangleRenderer_->addTriangle(std::move(triangle6));
        triangleRenderer_->addTriangle(std::move(triangle7));
        triangleRenderer_->addTriangle(std::move(triangle8));
        triangleRenderer_->addTriangle(std::move(triangle9));
        triangleRenderer_->addTriangle(std::move(triangle10));
        triangleRenderer_->addTriangle(std::move(triangle11));
        triangleRenderer_->addTriangle(std::move(triangle12));
        triangleRenderer_->addTriangle(std::move(triangle13));
        triangleRenderer_->addTriangle(std::move(triangle14));
        triangleRenderer_->addTriangle(std::move(triangle15));
        triangleRenderer_->addTriangle(std::move(triangle16));
        triangleRenderer_->addTriangle(std::move(triangle17));
        
        std::cout << "Created " << 8 << " triangles" << std::endl;
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
            const std::vector<std::unique_ptr<Triangle>>& triangles = triangleRenderer_->getTriangles();
            for(uint32_t i = 0; i < triangles.size(); i++){
                processInput(window_.getHandle(), *triangles[i], 0.01f);
            }
            drawFrame();
        }
        
        vkDeviceWaitIdle(context_->getDevice());
    }

    void drawFrame() {
        std::cout << "=== Starting frame ===" << std::endl;

        uint32_t imageIndex;

        std::cout << "Acquiring next image..." << std::endl;
        VkResult acquireResult = swapchain_->acquireNextImage(imageAvailableSemaphore_, &imageIndex);
        if (acquireResult != VK_SUCCESS && acquireResult != VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error("Failed to acquire swap chain image");
        }
        std::cout << "Acquired image index: " << imageIndex << std::endl;


        std::cout << "Recording triangle rendering (includes clear)..." << std::endl;
        commandBuffer_->recordTriangleRendering(imageIndex, *triangleRenderer_);

        std::cout << "Submitting command buffer..." << std::endl;
        commandBuffer_->submit(imageIndex, imageAvailableSemaphore_, renderFinishedSemaphore_);

        std::cout << "Presenting..." << std::endl;
        swapchain_->present(imageIndex, renderFinishedSemaphore_);

        std::cout << "Waiting for queue idle..." << std::endl;
        vkQueueWaitIdle(context_->getGraphicsQueue());

        std::cout << "=== Frame completed ===" << std::endl;
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
    void processInput(GLFWwindow* window, Triangle& triangle, float deltaTime) {
        const float moveSpeed = 0.5f * deltaTime;
        const float rotateSpeed = 90.0f * deltaTime;
        const float scaleSpeed = 1.0f + 0.5f * deltaTime;

        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
            triangle.move({-moveSpeed, 0.0f});
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
            triangle.move({moveSpeed, 0.0f});
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
            triangle.move({0.0f, moveSpeed});
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
            triangle.move({0.0f, -moveSpeed});

        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
            triangle.rotate(rotateSpeed);
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
            triangle.rotate(-rotateSpeed);

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            triangle.scale({scaleSpeed, scaleSpeed});
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            triangle.scale({1.0f / scaleSpeed, 1.0f / scaleSpeed});
    }
};



int main() {
    VulkanTriangleApp app;
    
    try {
        app.run();
        std::cout << "Application finished successfully" << std::endl;
        return EXIT_SUCCESS;
    } catch (const std::exception& e) {
        std::cerr << "Application error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}
