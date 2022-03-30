#pragma once

#include <vulkan/vulkan.h>

class GraphicsPipeline {
public:
    GraphicsPipeline(VkRenderPass renderPass);

private:
    VkShaderModule m_vertShaderModule = VK_NULL_HANDLE;
    VkShaderModule m_fragShaderModule = VK_NULL_HANDLE;
};
