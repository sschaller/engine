#pragma once

#include <memory>
#include "Object.h"
#include "../materials/Material.h"

class MeshObject final : public Object {
public:
    void SetMaterial(const std::shared_ptr<Material> &rMaterial);
    std::shared_ptr<Material> GetMaterial() const;

    void Update(DeviceContext &rDeviceContext, Swapchain &rSwapchain, VkRenderPass &rRenderPass, VkFormat &rImageFormat, VkCommandBuffer &rCommandBuffer, bool outOfDate) override;
    void Draw(VkCommandBuffer &rCommandBuffer) override;

private:
    std::shared_ptr<Material> material_;
};
