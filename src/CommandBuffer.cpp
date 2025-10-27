#include "CommandBuffer.hpp"
#include "TriangleRenderer.hpp"
#include <stdexcept>
#include <iostream>

CommandBuffer::CommandBuffer(VulkanContext& context, Swapchain& swapchain) 
    : context_(context), swapchain_(swapchain) {
    createCommandPool();
    createCommandBuffer();
}

CommandBuffer::~CommandBuffer() {
    if (commandPool_ != VK_NULL_HANDLE) {
        vkDestroyCommandPool(context_.getDevice(), commandPool_, nullptr);
    }
}

void CommandBuffer::createCommandPool() {
    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = context_.getGraphicsQueueFamily();
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    
    if (vkCreateCommandPool(context_.getDevice(), &poolInfo, nullptr, &commandPool_) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create command pool");
    }
}

void CommandBuffer::createCommandBuffer() {
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool_;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = 1;
    
    if (vkAllocateCommandBuffers(context_.getDevice(), &allocInfo, &commandBuffer_) != VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate command buffers");
    }
}


void CommandBuffer::recordTriangleRendering(uint32_t imageIndex, TriangleRenderer& renderer) {
    std::cout << "CommandBuffer: Starting recordTriangleRendering..." << std::endl;

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    std::cout << "CommandBuffer: Beginning command buffer..." << std::endl;
    if (vkBeginCommandBuffer(commandBuffer_, &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("Failed to begin recording command buffer");
    }


    std::cout << "CommandBuffer: Calling renderer..." << std::endl;
    renderer.render(commandBuffer_, imageIndex);

    std::cout << "CommandBuffer: Ending command buffer..." << std::endl;
    if (vkEndCommandBuffer(commandBuffer_) != VK_SUCCESS) {
        throw std::runtime_error("Failed to record command buffer");
    }

    std::cout << "CommandBuffer: recordTriangleRendering completed" << std::endl;
}

void CommandBuffer::submit(uint32_t imageIndex, VkSemaphore waitSemaphore, VkSemaphore signalSemaphore) {
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = &waitSemaphore;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer_;
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &signalSemaphore;
    
    if (vkQueueSubmit(context_.getGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS) {
        throw std::runtime_error("Failed to submit draw command buffer");
    }
}

