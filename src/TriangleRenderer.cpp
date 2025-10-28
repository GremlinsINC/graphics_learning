#include "TriangleRenderer.hpp"
#include <stdexcept>
#include <iostream>
#include <array>
#include <cstring>
#include <fstream>

TriangleRenderer::TriangleRenderer(VulkanContext& context, Swapchain& swapchain) 
    : context_(context), swapchain_(swapchain) {
    std::cout << "TriangleRenderer: Starting initialization..." << std::endl;
    
    try {
        std::cout << "TriangleRenderer: Creating render pass..." << std::endl;
        createRenderPass();
        
        std::cout << "TriangleRenderer: Creating framebuffers..." << std::endl;
        createFramebuffers();
        
        std::cout << "TriangleRenderer: Creating graphics pipeline..." << std::endl;
        createGraphicsPipeline();
        
        std::cout << "TriangleRenderer: Creating vertex buffer..." << std::endl;
        createVertexBuffer();
        
        std::cout << "TriangleRenderer: Creating index buffer..." << std::endl;
        createIndexBuffer();
        
        std::cout << "TriangleRenderer: Initialization completed successfully" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "TriangleRenderer: Error during initialization: " << e.what() << std::endl;
        throw;
    }
}

TriangleRenderer::~TriangleRenderer() {
    std::cout << "TriangleRenderer: Destroying..." << std::endl;
    
    VkDevice device = context_.getDevice();
    
    // Уничтожаем framebuffers
    for (auto framebuffer : framebuffers_) {
        vkDestroyFramebuffer(device, framebuffer, nullptr);
    }
    
    // Уничтожаем image views
    for (auto imageView : imageViews_) {
        vkDestroyImageView(device, imageView, nullptr);
    }
    
    if (graphicsPipeline_ != VK_NULL_HANDLE) {
        vkDestroyPipeline(device, graphicsPipeline_, nullptr);
        graphicsPipeline_ = VK_NULL_HANDLE;
    }
    if (pipelineLayout_ != VK_NULL_HANDLE) {
        vkDestroyPipelineLayout(device, pipelineLayout_, nullptr);
        pipelineLayout_ = VK_NULL_HANDLE;
    }
    if (renderPass_ != VK_NULL_HANDLE) {
        vkDestroyRenderPass(device, renderPass_, nullptr);
        renderPass_ = VK_NULL_HANDLE;
    }
    if (vertexBuffer_ != VK_NULL_HANDLE) {
        vkDestroyBuffer(device, vertexBuffer_, nullptr);
        vertexBuffer_ = VK_NULL_HANDLE;
    }
    if (vertexBufferMemory_ != VK_NULL_HANDLE) {
        vkFreeMemory(device, vertexBufferMemory_, nullptr);
        vertexBufferMemory_ = VK_NULL_HANDLE;
    }
    if (indexBuffer_ != VK_NULL_HANDLE) {
        vkDestroyBuffer(device, indexBuffer_, nullptr);
        indexBuffer_ = VK_NULL_HANDLE;
    }
    if (indexBufferMemory_ != VK_NULL_HANDLE) {
        vkFreeMemory(device, indexBufferMemory_, nullptr);
        indexBufferMemory_ = VK_NULL_HANDLE;
    }
}

void TriangleRenderer::addTriangle(std::unique_ptr<Triangle> triangle) {
    triangles_.push_back(std::move(triangle));
}

void TriangleRenderer::render(VkCommandBuffer commandBuffer, uint32_t imageIndex) {
    if (triangles_.empty()) {
        return;
    }

    updateVertexBuffer();

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = renderPass_;
    renderPassInfo.framebuffer = framebuffers_[imageIndex];
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = swapchain_.getExtent();

    VkClearValue clearColor = {{{1.0f, 1.0f, 1.0f, 1.0f}}};
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline_);

    VkBuffer vertexBuffers[] = {vertexBuffer_};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

    vkCmdBindIndexBuffer(commandBuffer, indexBuffer_, 0, VK_INDEX_TYPE_UINT16);

    vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(triangles_.size() * 3), 1, 0, 0, 0);

    vkCmdEndRenderPass(commandBuffer);
}

void TriangleRenderer::createRenderPass() {
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = swapchain_.getImageFormat();
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    
    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    
    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;
    
    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &colorAttachment;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    
    if (vkCreateRenderPass(context_.getDevice(), &renderPassInfo, nullptr, &renderPass_) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create render pass!");
    }
}

void TriangleRenderer::createFramebuffers() {
    auto& images = swapchain_.getImages();
    framebuffers_.resize(images.size());
    imageViews_.resize(images.size());
    
    for (size_t i = 0; i < images.size(); i++) {
        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = images[i];
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = swapchain_.getImageFormat();
        viewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;
        
        if (vkCreateImageView(context_.getDevice(), &viewInfo, nullptr, &imageViews_[i]) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create image view for framebuffer");
        }
        
        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = renderPass_;
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments = &imageViews_[i];
        framebufferInfo.width = swapchain_.getExtent().width;
        framebufferInfo.height = swapchain_.getExtent().height;
        framebufferInfo.layers = 1;
        
        if (vkCreateFramebuffer(context_.getDevice(), &framebufferInfo, nullptr, &framebuffers_[i]) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create framebuffer");
        }
    }
    
    std::cout << "TriangleRenderer: Created " << framebuffers_.size() << " framebuffers" << std::endl;
}

void TriangleRenderer::createGraphicsPipeline() {
    // Читаем шейдеры из файлов
    auto vertShaderCode = readFile("shaders/simple.vert.spv");
    auto fragShaderCode = readFile("shaders/simple.frag.spv");

    std::cout << "TriangleRenderer: shader files readed!" << std::endl;

    VkShaderModule vertShaderModule = createShaderModule(vertShaderCode);
    VkShaderModule fragShaderModule = createShaderModule(fragShaderCode);

    // Настраиваем шейдерные стадии
    VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vertShaderModule;
    vertShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = fragShaderModule;
    fragShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

    std::cout << "TriangleRenderer: shaders stage is configured!" << std::endl;
    // Настраиваем vertex input
    auto bindingDescription = Vertex::getBindingDescription();
    auto attributeDescriptions = Vertex::getAttributeDescriptions();

    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

    std::cout << "TriangleRenderer: vertex input is configured!" << std::endl;

    // Настраиваем input assembly
    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    std::cout << "TriangleRenderer: input assembly is configured!" << std::endl;

    // Настраиваем viewport и scissor
    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)swapchain_.getExtent().width;
    viewport.height = (float)swapchain_.getExtent().height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = swapchain_.getExtent();

    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.pViewports = &viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissor;

    std::cout << "TriangleRenderer: viewport and scissor is configured!" << std::endl;

    // Настраиваем rasterizer
    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.polygonMode = VK_POLYGON_MODE_LINE;
    rasterizer.lineWidth = 2.0f;
    rasterizer.cullMode = VK_CULL_MODE_NONE;
    rasterizer.lineWidth = 1.0f;
    //rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.cullMode = VK_CULL_MODE_NONE;
    rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;

    std::cout << "TriangleRenderer: rasterizer is configured!" << std::endl;

    // Настраиваем multisampling
    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    std::cout << "TriangleRenderer: multisampling is configured!" << std::endl;

    // Настраиваем color blending
    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;

    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f;
    colorBlending.blendConstants[1] = 0.0f;
    colorBlending.blendConstants[2] = 0.0f;
    colorBlending.blendConstants[3] = 0.0f;

    std::cout << "TriangleRenderer: color blending is configured!" << std::endl;

    // Создаем pipeline layout
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 0;
    pipelineLayoutInfo.pushConstantRangeCount = 0;

    if (vkCreatePipelineLayout(context_.getDevice(), &pipelineLayoutInfo, nullptr, &pipelineLayout_) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create pipeline layout!");
    }

    std::cout << "TriangleRenderer: pipeline layout is created!" << std::endl;

    // Создаем графический пайплайн
    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.layout = pipelineLayout_;
    pipelineInfo.renderPass = renderPass_;
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

    if (vkCreateGraphicsPipelines(context_.getDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline_) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create graphics pipeline!");
    }

    std::cout << "TriangleRenderer: graphics pipeline is created!" << std::endl;

    vkDestroyShaderModule(context_.getDevice(), fragShaderModule, nullptr);
    vkDestroyShaderModule(context_.getDevice(), vertShaderModule, nullptr);
    
    std::cout << "TriangleRenderer: Graphics pipeline created successfully" << std::endl;
}

void TriangleRenderer::createVertexBuffer() {
    VkDeviceSize bufferSize = sizeof(Vertex) * 100;
    
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = bufferSize;
    bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    
    VkResult result = vkCreateBuffer(context_.getDevice(), &bufferInfo, nullptr, &vertexBuffer_);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed to create vertex buffer");
    }
    
    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(context_.getDevice(), vertexBuffer_, &memRequirements);
    
    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(context_.getPhysicalDevice(), &memProperties);
    
    uint32_t memoryTypeIndex = UINT32_MAX;
    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((memRequirements.memoryTypeBits & (1 << i)) && 
            (memProperties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) &&
            (memProperties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)) {
            memoryTypeIndex = i;
            break;
        }
    }
    
    if (memoryTypeIndex == UINT32_MAX) {
        throw std::runtime_error("Failed to find suitable memory type for vertex buffer");
    }
    
    allocInfo.memoryTypeIndex = memoryTypeIndex;
    
    result = vkAllocateMemory(context_.getDevice(), &allocInfo, nullptr, &vertexBufferMemory_);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate vertex buffer memory");
    }
    
    result = vkBindBufferMemory(context_.getDevice(), vertexBuffer_, vertexBufferMemory_, 0);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed to bind vertex buffer memory");
    }
}

void TriangleRenderer::createIndexBuffer() {
    std::vector<uint16_t> indices(100 * 3);
    for (uint16_t i = 0; i < 100; ++i) {
        indices[i * 3 + 0] = i * 3 + 0;
        indices[i * 3 + 1] = i * 3 + 1;
        indices[i * 3 + 2] = i * 3 + 2;
    }
    
    VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();
    
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = bufferSize;
    bufferInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    
    VkResult result = vkCreateBuffer(context_.getDevice(), &bufferInfo, nullptr, &indexBuffer_);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed to create index buffer");
    }
    
    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(context_.getDevice(), indexBuffer_, &memRequirements);
    
    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(context_.getPhysicalDevice(), &memProperties);
    
    uint32_t memoryTypeIndex = UINT32_MAX;
    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((memRequirements.memoryTypeBits & (1 << i)) && 
            (memProperties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) &&
            (memProperties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)) {
            memoryTypeIndex = i;
            break;
        }
    }
    
    if (memoryTypeIndex == UINT32_MAX) {
        throw std::runtime_error("Failed to find suitable memory type for index buffer");
    }
    
    allocInfo.memoryTypeIndex = memoryTypeIndex;
    
    result = vkAllocateMemory(context_.getDevice(), &allocInfo, nullptr, &indexBufferMemory_);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate index buffer memory");
    }
    
    result = vkBindBufferMemory(context_.getDevice(), indexBuffer_, indexBufferMemory_, 0);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed to bind index buffer memory");
    }
    
    void* data;
    result = vkMapMemory(context_.getDevice(), indexBufferMemory_, 0, bufferSize, 0, &data);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed to map index buffer memory");
    }
    
    memcpy(data, indices.data(), (size_t)bufferSize);
    vkUnmapMemory(context_.getDevice(), indexBufferMemory_);
}

void TriangleRenderer::updateVertexBuffer() {
    auto vertices = collectVertexData();
    if (vertices.empty()) return;
    
    VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();
    
    void* data;
    VkResult result = vkMapMemory(context_.getDevice(), vertexBufferMemory_, 0, bufferSize, 0, &data);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed to map vertex buffer memory");
    }
    
    memcpy(data, vertices.data(), (size_t)bufferSize);
    vkUnmapMemory(context_.getDevice(), vertexBufferMemory_);
}

std::vector<TriangleRenderer::Vertex> TriangleRenderer::collectVertexData() const {
    std::vector<Vertex> vertices;

    Vector2 screenSize = {
        static_cast<float>(swapchain_.getExtent().width),
        static_cast<float>(swapchain_.getExtent().height)
    };

    for (const auto& triangle : triangles_) {
        auto screenVertices = triangle->getScreenVertices(screenSize);
        auto color = triangle->getColor();

        for (const auto& vertex : screenVertices) {
            Vertex v;

            float ndcX = (vertex.x / screenSize.x) * 2.0f - 1.0f;
            float ndcY = 1.0f - (vertex.y / screenSize.y) * 2.0f;

            v.position[0] = ndcX;
            v.position[1] = ndcY;

            v.color[0] = color[0];
            v.color[1] = color[1];
            v.color[2] = color[2];
            v.color[3] = color[3];

            vertices.push_back(v);
        }
    }

    // Логируем первые несколько вершин (чтобы убедиться в нормализации)
    std::cout << "collectVertexData: total vertices = " << vertices.size() << std::endl;
    for (size_t i = 0; i < vertices.size() && i < 12; ++i) {
        std::cout << " V[" << i << "]: pos=("
                  << vertices[i].position[0] << ", " << vertices[i].position[1]
                  << "), color=("
                  << vertices[i].color[0] << "," << vertices[i].color[1] << ","
                  << vertices[i].color[2] << "," << vertices[i].color[3] << ")"
                  << std::endl;
    }

    return vertices;
}

VkShaderModule TriangleRenderer::createShaderModule(const std::vector<char>& code) {
    if (code.empty()) {
        throw std::runtime_error("createShaderModule: shader code is empty");
    }
    if (code.size() % 4 != 0) {
        throw std::runtime_error("createShaderModule: shader code size must be a multiple of 4 (not valid SPIR-V?)");
    }

    // Копируем байты в vector<uint32_t>, чтобы обеспечить корректное выравнивание.
    std::vector<uint32_t> codeAligned(code.size() / 4);
    std::memcpy(codeAligned.data(), code.data(), code.size());

    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size(); // size в байтах (кратно 4)
    createInfo.pCode = codeAligned.data();

    VkShaderModule shaderModule;
    if (vkCreateShaderModule(context_.getDevice(), &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create shader module!");
    }

    return shaderModule;
}

std::vector<char> TriangleRenderer::readFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::ate | std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + filename);
    }

    size_t fileSize = (size_t)file.tellg();
    if (fileSize == 0) {
        throw std::runtime_error("Shader file is empty: " + filename);
    }
    file.seekg(0);

    std::vector<char> buffer(fileSize);
    file.read(buffer.data(), fileSize);
    file.close();

    if (fileSize % 4 != 0) {
        std::cerr << "Warning: shader file size is not a multiple of 4 -> likely not SPIR-V binary: " << filename << std::endl;
    }

    return buffer;
}


const std::vector<std::unique_ptr<Triangle>>& TriangleRenderer::getTriangles(){
    return triangles_;
}
