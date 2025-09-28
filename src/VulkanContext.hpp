#pragma once

#include "types.hpp"
#include "Window.hpp"
#include <vector>
#include <string>

class VulkanContext {
public:
    VulkanContext(Window& window);
    ~VulkanContext();
    
    VulkanContext(const VulkanContext&) = delete;
    VulkanContext& operator=(const VulkanContext&) = delete;
    
    VkInstance getInstance() const { return instance_; }
    VkPhysicalDevice getPhysicalDevice() const { return physicalDevice_; }
    VkDevice getDevice() const { return device_; }
    VkQueue getGraphicsQueue() const { return graphicsQueue_; }
    VkQueue getPresentQueue() const { return presentQueue_; }
    VkSurfaceKHR getSurface() const { return surface_; }
    uint32_t getGraphicsQueueFamily() const { return queueFamilies_.graphicsFamily.value(); }
    uint32_t getPresentQueueFamily() const { return queueFamilies_.presentFamily.value(); }
    const QueueFamilyIndices& getQueueFamilyIndices() const { return queueFamilies_; }
    Window& getWindow() const { return window_; }
    
private:
    Window& window_;
    
    VkInstance instance_ = VK_NULL_HANDLE;
    VkPhysicalDevice physicalDevice_ = VK_NULL_HANDLE;
    VkDevice device_ = VK_NULL_HANDLE;
    VkSurfaceKHR surface_ = VK_NULL_HANDLE;
    
    VkQueue graphicsQueue_ = VK_NULL_HANDLE;
    VkQueue presentQueue_ = VK_NULL_HANDLE;
    
    QueueFamilyIndices queueFamilies_;
    
    void createInstance();
    void pickPhysicalDevice();
    void createLogicalDevice();
    void findQueueFamilies(VkPhysicalDevice device);
    
    bool checkDeviceExtensionSupport(VkPhysicalDevice device);
    bool isDeviceSuitable(VkPhysicalDevice device);
    
    std::vector<const char*> getRequiredExtensions() const;
};
