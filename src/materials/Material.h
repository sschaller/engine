#pragma once

#include <vulkan/vulkan.h>

class DeviceContext;
class Swapchain;

class Material
{
public:
    virtual ~Material() = default;
    
    virtual void Update(DeviceContext &rDeviceContext, Swapchain &rSwapchain, VkRenderPass &rRenderPass, VkFormat &rImageFormat, VkCommandBuffer &rCommandBuffer, bool outOfDate) = 0;
    virtual void Bind(VkCommandBuffer &rCommandBuffer) = 0;
};
