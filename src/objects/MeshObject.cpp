#include "MeshObject.h"

void MeshObject::SetMaterial(const std::shared_ptr<Material> &rMaterial)
{
    material_ = rMaterial;
}

std::shared_ptr<Material> MeshObject::GetMaterial() const
{
    return material_;
}

void MeshObject::Update(DeviceContext &rDeviceContext, Swapchain &rSwapchain, VkRenderPass &rRenderPass,
                        VkFormat &rImageFormat, VkCommandBuffer &rCommandBuffer, bool outOfDate) {
    if(material_ == nullptr)
    {
        return;
    }

    material_->Update(rDeviceContext, rSwapchain, rRenderPass, rImageFormat, rCommandBuffer, outOfDate);
}

void MeshObject::Draw(VkCommandBuffer &rCommandBuffer) {
    if(material_ == nullptr)
    {
        return;
    }

    /*
    VkBuffer vertexBuffers[] = {m_vertexBuffer};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(m_commandBuffers[i], 0, 1, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(m_commandBuffers[i], m_indexBuffer, 0, VK_INDEX_TYPE_UINT16);
    */

    // Bind graphics pipeline
    material_->Bind(rCommandBuffer);
    // Draw
    // vkCmdDrawIndexed(m_commandBuffers[i], static_cast<uint32_t>(c_indices.size()), 1, 0, 0, 0);
    vkCmdDraw(rCommandBuffer, 3, 1, 0, 0);
}
