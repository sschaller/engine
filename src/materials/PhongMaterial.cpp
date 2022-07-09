#include "PhongMaterial.h"

#include "../GraphicsPipeline.h"
#include "../ResourceManager.h"

PhongMaterial::~PhongMaterial() = default;

void PhongMaterial::Update(DeviceContext &rDeviceContext, Swapchain &rSwapchain, VkRenderPass &rRenderPass,
                           VkFormat &rImageFormat, VkCommandBuffer &rCommandBuffer, bool outOfDate) {
    if (spPipeline_ == nullptr || outOfDate) {
        // Recreate pipeline
        spPipeline_ = std::make_unique<GraphicsPipeline>(rDeviceContext, rSwapchain, rRenderPass, rImageFormat);

        GraphicsPipeline::ShaderModule vertexModule{VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT,
                                                    ResourceManager::ReadBinaryFile("shaders/shader.vert.spv")};
        GraphicsPipeline::ShaderModule fragmentModule{VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT,
                                                      ResourceManager::ReadBinaryFile("shaders/shader.frag.spv")};

        spPipeline_->SetShaderModules({vertexModule, fragmentModule});
    }

    spPipeline_->Update();
}

void PhongMaterial::Bind(VkCommandBuffer &rCommandBuffer) { spPipeline_->Bind(rCommandBuffer); }
