#pragma once

#include <vulkan/vulkan.h>

#include "DeviceContext.h"
#include "Swapchain.h"

struct RenderContext {
    DeviceContext &deviceContext;
    Swapchain &swapchain;
    VkRenderPass &renderPass;
    VkFormat &imageFormat;
    VkCommandBuffer &commandBuffer;
    uint32_t imageIndex = 0;
    bool outOfDate = 0;
};
