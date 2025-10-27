#pragma once

#include "VulkanContext.hpp"
#include "Swapchain.hpp"
#include "Triangle.hpp"
#include <vector>
#include <memory>

class TriangleRenderer {
public:
    TriangleRenderer(VulkanContext& context, Swapchain& swapchain);
    ~TriangleRenderer();
    
    TriangleRenderer(const TriangleRenderer&) = delete;
    TriangleRenderer& operator=(const TriangleRenderer&) = delete;
    
    void addTriangle(std::unique_ptr<Triangle> triangle);
    void render(VkCommandBuffer commandBuffer, uint32_t imageIndex);
    
    const std::vector<std::unique_ptr<Triangle>>& getTriangles();
    
private:
    VulkanContext& context_;
    Swapchain& swapchain_;
    
    std::vector<std::unique_ptr<Triangle>> triangles_;
    std::vector<VkFramebuffer> framebuffers_;
    std::vector<VkImageView> imageViews_;  // Для хранения image views
    
    VkPipeline graphicsPipeline_ = VK_NULL_HANDLE;
    VkPipelineLayout pipelineLayout_ = VK_NULL_HANDLE;
    VkRenderPass renderPass_ = VK_NULL_HANDLE;
    
    VkBuffer vertexBuffer_ = VK_NULL_HANDLE;
    VkDeviceMemory vertexBufferMemory_ = VK_NULL_HANDLE;
    
    VkBuffer indexBuffer_ = VK_NULL_HANDLE;
    VkDeviceMemory indexBufferMemory_ = VK_NULL_HANDLE;
    
    struct Vertex {
        float position[2];
        float color[4];
        
        static VkVertexInputBindingDescription getBindingDescription() {
            VkVertexInputBindingDescription bindingDescription{};
            bindingDescription.binding = 0;
            bindingDescription.stride = sizeof(Vertex);
            bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
            return bindingDescription;
        }
        
        static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions() {
            std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};
            
            // Position attribute
            attributeDescriptions[0].binding = 0;
            attributeDescriptions[0].location = 0;
            attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
            attributeDescriptions[0].offset = offsetof(Vertex, position);
            
            // Color attribute
            attributeDescriptions[1].binding = 0;
            attributeDescriptions[1].location = 1;
            attributeDescriptions[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
            attributeDescriptions[1].offset = offsetof(Vertex, color);
            
            return attributeDescriptions;
        }
    };
    
    void createRenderPass();
    void createFramebuffers();
    void createGraphicsPipeline();
    void createVertexBuffer();
    void createIndexBuffer();
    void updateVertexBuffer();
    
    std::vector<Vertex> collectVertexData() const;
    
    VkShaderModule createShaderModule(const std::vector<char>& code);
    std::vector<char> readFile(const std::string& filename);
};
