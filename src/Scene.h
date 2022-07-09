#pragma once

#include <memory>
#include <vector>
#include <vulkan/vulkan.h>
#include "objects/Object.h"

class Scene
{
public:
    void AddObject(std::unique_ptr<Object> &&rrObject);
    void RemoveObject(const Object &rObject);
    
    void Update(DeviceContext &rDeviceContext, Swapchain &rSwapchain, VkRenderPass &rRenderPass, VkFormat &rImageFormat, VkCommandBuffer &rCommandBuffer, bool outOfDate);
    void Draw(VkCommandBuffer &rCommandBuffer);
private:
    std::vector<std::unique_ptr<Object>> objects_;
};
