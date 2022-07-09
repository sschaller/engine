#pragma once

#include <memory>
#include "Material.h"

class GraphicsPipeline;

class PhongMaterial : public Material {
public:
    ~PhongMaterial() override;
    void Update(DeviceContext &rDeviceContext, Swapchain &rSwapchain, VkRenderPass &rRenderPass, VkFormat &rImageFormat, VkCommandBuffer &rCommandBuffer, bool outOfDate) override;
    void Bind(VkCommandBuffer &rCommandBuffer) override;

private:
    std::unique_ptr<GraphicsPipeline> spPipeline_;
};
