#include "MeshObject.h"
#include "../RenderContext.h"
#include "../ResourceManager.h"

MeshObject::~MeshObject() = default;

void MeshObject::SetMaterial(const std::shared_ptr<Material> &rMaterial)
{
    material_ = rMaterial;
}

std::shared_ptr<Material> MeshObject::GetMaterial() const
{
    return material_;
}

void MeshObject::SetVertexData(const std::shared_ptr<VertexData> &rVertexData)
{
    vertexData_ = rVertexData;
}

void MeshObject::Update(const RenderContext &rContext) {
    if(material_ == nullptr)
    {
        return;
    }

    material_->Update(rContext);

    if(vertexData_ != nullptr){
        // Create vertex buffer
    }

}

void MeshObject::Draw(const RenderContext &rContext) {
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
    material_->Bind(rContext.commandBuffer);
    // Draw
    // vkCmdDrawIndexed(m_commandBuffers[i], static_cast<uint32_t>(c_indices.size()), 1, 0, 0, 0);
    vkCmdDraw(rContext.commandBuffer, 3, 1, 0, 0);
}
