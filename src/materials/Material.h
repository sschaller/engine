#pragma once

#include <vulkan/vulkan.h>

class DeviceContext;
class RenderContext;
class Swapchain;

class Material
{
public:
    virtual ~Material() = default;
    
    virtual void Update(const RenderContext &rContext) = 0;
    virtual void Bind(VkCommandBuffer &rCommandBuffer) = 0;
};
