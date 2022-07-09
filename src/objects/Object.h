#pragma once

#include <vulkan/vulkan.h>

class DeviceContext;
class Swapchain;

class Object {
public:
    virtual ~Object() = default;
    virtual void Update(DeviceContext &rDeviceContext, Swapchain &rSwapchain, VkRenderPass &rRenderPass, VkFormat &rImageFormat, VkCommandBuffer &rCommandBuffer, bool outOfDate) = 0;
    virtual void Draw(VkCommandBuffer &rCommandBuffer) = 0;
};
