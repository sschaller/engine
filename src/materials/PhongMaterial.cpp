#include "PhongMaterial.h"

#include "../GraphicsPipeline.h"
#include "../RenderContext.h"
#include "../ResourceManager.h"

PhongMaterial::~PhongMaterial() = default;

void PhongMaterial::Update(const RenderContext &rContext) {
    if (spPipeline_ == nullptr || rContext.outOfDate) {
        // Recreate pipeline
        spPipeline_ = std::make_unique<GraphicsPipeline>(rContext.deviceContext, rContext.swapchain, rContext.renderPass, rContext.imageFormat);

        GraphicsPipeline::ShaderModule vertexModule{VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT,
                                                    ResourceManager::ReadBinaryFile("shaders/shader.vert.spv")};
        GraphicsPipeline::ShaderModule fragmentModule{VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT,
                                                      ResourceManager::ReadBinaryFile("shaders/shader.frag.spv")};

        spPipeline_->SetShaderModules({vertexModule, fragmentModule});
    }

    spPipeline_->Update();
}

void PhongMaterial::Bind(VkCommandBuffer &rCommandBuffer) { spPipeline_->Bind(rCommandBuffer); }
