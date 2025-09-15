#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define WIDTH 800
#define HEIGHT 600

int main() {
    if (!glfwInit()) {
        fprintf(stderr, "Failed to init GLFW\n");
        return -1;
    }
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // только Vulkan
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "GLFW Vulkan Wayland", NULL, NULL);
    if (!window) {
        fprintf(stderr, "Failed to create window\n");
        glfwTerminate();
        return -1;
    }

    VkApplicationInfo appInfo = { .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO };
    appInfo.pApplicationName = "GLFW Vulkan Clear";
    appInfo.applicationVersion = VK_MAKE_VERSION(1,0,0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    uint32_t extCount = 0;
    const char** ext = glfwGetRequiredInstanceExtensions(&extCount);

    VkInstanceCreateInfo ci = { .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
    ci.pApplicationInfo = &appInfo;
    ci.enabledExtensionCount = extCount;
    ci.ppEnabledExtensionNames = ext;

    VkInstance instance;
    if (vkCreateInstance(&ci, NULL, &instance) != VK_SUCCESS) {
        fprintf(stderr, "vkCreateInstance failed\n");
        return -1;
    }

    VkSurfaceKHR surface;
    if (glfwCreateWindowSurface(instance, window, NULL, &surface) != VK_SUCCESS) {
        fprintf(stderr, "glfwCreateWindowSurface failed\n");
        return -1;
    }

    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, NULL);
    if (deviceCount == 0) {
        fprintf(stderr, "No Vulkan devices found\n");
        return -1;
    }
    VkPhysicalDevice phys;
    vkEnumeratePhysicalDevices(instance, &deviceCount, &phys);

    uint32_t qCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(phys, &qCount, NULL);
    VkQueueFamilyProperties qProps[qCount];
    vkGetPhysicalDeviceQueueFamilyProperties(phys, &qCount, qProps);

    int gfxQueueFamily = -1;
    for (uint32_t i=0; i<qCount; i++) {
        VkBool32 presentSupport = 0;
        vkGetPhysicalDeviceSurfaceSupportKHR(phys, i, surface, &presentSupport);
        if ((qProps[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) && presentSupport) {
            gfxQueueFamily = i; break;
        }
    }
    if (gfxQueueFamily < 0) {
        fprintf(stderr, "No suitable queue family\n");
        return -1;
    }

    float qPriority = 1.0f;
    VkDeviceQueueCreateInfo qci = { .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO };
    qci.queueFamilyIndex = gfxQueueFamily;
    qci.queueCount = 1;
    qci.pQueuePriorities = &qPriority;

    const char* devExt[] = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
    VkDeviceCreateInfo dci = { .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };
    dci.queueCreateInfoCount = 1;
    dci.pQueueCreateInfos = &qci;
    dci.enabledExtensionCount = 1;
    dci.ppEnabledExtensionNames = devExt;

    VkDevice device;
    if (vkCreateDevice(phys, &dci, NULL, &device) != VK_SUCCESS) {
        fprintf(stderr, "vkCreateDevice failed\n");
        return -1;
    }
    VkQueue queue;
    vkGetDeviceQueue(device, gfxQueueFamily, 0, &queue);

    VkSurfaceCapabilitiesKHR caps;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(phys, surface, &caps);

    uint32_t fmtCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(phys, surface, &fmtCount, NULL);
    VkSurfaceFormatKHR formats[fmtCount];
    vkGetPhysicalDeviceSurfaceFormatsKHR(phys, surface, &fmtCount, formats);
    VkSurfaceFormatKHR chosenFormat = formats[0];

    VkExtent2D extent = caps.currentExtent.width != UINT32_MAX ? 
                        caps.currentExtent : (VkExtent2D){WIDTH, HEIGHT};

    uint32_t minImg = caps.minImageCount + 1;
    if (caps.maxImageCount > 0 && minImg > caps.maxImageCount) minImg = caps.maxImageCount;

    VkSwapchainCreateInfoKHR sci = { .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR };
    sci.surface = surface;
    sci.minImageCount = minImg;
    sci.imageFormat = chosenFormat.format;
    sci.imageColorSpace = chosenFormat.colorSpace;
    sci.imageExtent = extent;
    sci.imageArrayLayers = 1;
    sci.imageUsage = VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    sci.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    sci.preTransform = caps.currentTransform;
    sci.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    sci.presentMode = VK_PRESENT_MODE_FIFO_KHR;
    sci.clipped = VK_TRUE;

    VkSwapchainKHR swapchain;
    if (vkCreateSwapchainKHR(device, &sci, NULL, &swapchain) != VK_SUCCESS) {
        fprintf(stderr, "vkCreateSwapchain failed\n");
        return -1;
    }

    uint32_t imgCount;
    vkGetSwapchainImagesKHR(device, swapchain, &imgCount, NULL);
    VkImage images[imgCount];
    vkGetSwapchainImagesKHR(device, swapchain, &imgCount, images);

    VkCommandPoolCreateInfo pci = { .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };
    pci.queueFamilyIndex = gfxQueueFamily;
    VkCommandPool pool;
    vkCreateCommandPool(device, &pci, NULL, &pool);

    VkCommandBufferAllocateInfo ai = { .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
    ai.commandPool = pool;
    ai.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    ai.commandBufferCount = 1;

    VkCommandBuffer cmd;
    vkAllocateCommandBuffers(device, &ai, &cmd);

    VkSemaphoreCreateInfo sciSem = { .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
    VkSemaphore imgAvailable, renderFinished;
    vkCreateSemaphore(device, &sciSem, NULL, &imgAvailable);
    vkCreateSemaphore(device, &sciSem, NULL, &renderFinished);

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        uint32_t idx;
        vkAcquireNextImageKHR(device, swapchain, UINT64_MAX, imgAvailable, VK_NULL_HANDLE, &idx);

        VkCommandBufferBeginInfo bi = { .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
        vkBeginCommandBuffer(cmd, &bi);

        VkImageMemoryBarrier barrier = { .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
        barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = images[idx];
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.levelCount = 1;
        barrier.subresourceRange.layerCount = 1;
        vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
                             0, 0,NULL, 0,NULL, 1,&barrier);

        VkClearColorValue color = { .float32 = {1.0f, 1.0f, 1.0f, 1.0f} };
        VkImageSubresourceRange range = barrier.subresourceRange;
        vkCmdClearColorImage(cmd, images[idx], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &color, 1, &range);

        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = 0;
        vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
                             0, 0,NULL, 0,NULL, 1,&barrier);

        vkEndCommandBuffer(cmd);

        VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        VkSubmitInfo si = { .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO };
        si.waitSemaphoreCount = 1;
        si.pWaitSemaphores = &imgAvailable;
        si.pWaitDstStageMask = &waitStage;
        si.commandBufferCount = 1;
        si.pCommandBuffers = &cmd;
        si.signalSemaphoreCount = 1;
        si.pSignalSemaphores = &renderFinished;

        vkQueueSubmit(queue, 1, &si, VK_NULL_HANDLE);

        VkPresentInfoKHR pi = { .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR };
        pi.waitSemaphoreCount = 1;
        pi.pWaitSemaphores = &renderFinished;
        pi.swapchainCount = 1;
        pi.pSwapchains = &swapchain;
        pi.pImageIndices = &idx;

        vkQueuePresentKHR(queue, &pi);
        vkQueueWaitIdle(queue);
    }

    vkDeviceWaitIdle(device);
    vkDestroySemaphore(device, imgAvailable, NULL);
    vkDestroySemaphore(device, renderFinished, NULL);
    vkDestroyCommandPool(device, pool, NULL);
    vkDestroySwapchainKHR(device, swapchain, NULL);
    vkDestroyDevice(device, NULL);
    vkDestroySurfaceKHR(instance, surface, NULL);
    vkDestroyInstance(instance, NULL);
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

