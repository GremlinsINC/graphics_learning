#pragma once

#include "VulkanContext.hpp"
#include "Swapchain.hpp"
#include <array>

class TriangleRenderer;

class CommandBuffer {
public:
    CommandBuffer(VulkanContext& context, Swapchain& swapchain);
    ~CommandBuffer();
    
    CommandBuffer(const CommandBuffer&) = delete;
    CommandBuffer& operator=(const CommandBuffer&) = delete;
    
    void recordTriangleRendering(uint32_t imageIndex, TriangleRenderer& renderer);
    void submit(uint32_t imageIndex, VkSemaphore waitSemaphore, VkSemaphore signalSemaphore);
    
    VkCommandBuffer getHandle() const { return commandBuffer_; }
    
private:
    VulkanContext& context_;
    Swapchain& swapchain_;
    
    VkCommandPool commandPool_ = VK_NULL_HANDLE;
    VkCommandBuffer commandBuffer_ = VK_NULL_HANDLE;
    
    void createCommandPool();
    void createCommandBuffer();
    
};
