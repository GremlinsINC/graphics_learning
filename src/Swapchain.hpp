#pragma once

#include "types.hpp"
#include "VulkanContext.hpp"
#include <vector>

class Swapchain {
public:
    Swapchain(VulkanContext& context);
    ~Swapchain();
    
    Swapchain(const Swapchain&) = delete;
    Swapchain& operator=(const Swapchain&) = delete;
    
    VkSwapchainKHR getHandle() const { return swapchain_; }
    VkFormat getImageFormat() const { return imageFormat_; }
    VkExtent2D getExtent() const { return extent_; }
    const std::vector<VkImage>& getImages() const { return images_; }
    
    // Изменяем возвращаемый тип на VkResult
    VkResult acquireNextImage(VkSemaphore imageAvailableSemaphore, uint32_t* imageIndex);
    void present(uint32_t imageIndex, VkSemaphore renderFinishedSemaphore);
    
private:
    VulkanContext& context_;
    
    VkSwapchainKHR swapchain_ = VK_NULL_HANDLE;
    std::vector<VkImage> images_;
    VkFormat imageFormat_;
    VkExtent2D extent_;
    
    void createSwapchain();
    void querySwapchainSupport(SwapchainSupportDetails& details) const;
    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
};
