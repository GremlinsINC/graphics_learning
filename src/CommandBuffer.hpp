#pragma once

#include "VulkanContext.hpp"
#include <array>

// Предварительное объявление вместо включения
class Swapchain;

class CommandBuffer {
public:
    CommandBuffer(VulkanContext& context, Swapchain& swapchain);
    ~CommandBuffer();
    
    CommandBuffer(const CommandBuffer&) = delete;
    CommandBuffer& operator=(const CommandBuffer&) = delete;
    
    void recordClearCommand(uint32_t imageIndex, const std::array<float, 4>& clearColor);
    void submit(uint32_t imageIndex, VkSemaphore waitSemaphore, VkSemaphore signalSemaphore);
    
    VkCommandBuffer getHandle() const { return commandBuffer_; }
    
private:
    VulkanContext& context_;
    Swapchain& swapchain_;
    
    VkCommandPool commandPool_ = VK_NULL_HANDLE;
    VkCommandBuffer commandBuffer_ = VK_NULL_HANDLE;
    
    void createCommandPool();
    void createCommandBuffer();
    
    void transitionImageLayout(VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout);
};
