#include "GraphicsPipeline.h"

#include <array>
#include <cassert>
#include <glm/glm.hpp>
#include <stdexcept>

#include "DeviceContext.h"
#include "Swapchain.h"

GraphicsPipeline::GraphicsPipeline(DeviceContext &rDeviceContext, Swapchain &rSwapchain, VkRenderPass renderPass,
                                   VkFormat imageFormat)
    : deviceContext_(rDeviceContext), renderPass_(renderPass), extent_(rSwapchain.GetExtent2D()) {}

GraphicsPipeline::~GraphicsPipeline() {
    vkDestroyPipeline(deviceContext_.GetDevice(), pipeline_, nullptr);
    vkDestroyPipelineLayout(deviceContext_.GetDevice(), pipelineLayout_, nullptr);
    vkDestroyDescriptorSetLayout(deviceContext_.GetDevice(), descriptorSetLayout_, nullptr);
    destroyShaderModules();
}

VkShaderModule GraphicsPipeline::createShaderModule(const std::vector<char> &rCode) {
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = rCode.size();
    createInfo.pCode = reinterpret_cast<const uint32_t *>(rCode.data());
    VkShaderModule shaderModule;
    if (vkCreateShaderModule(deviceContext_.GetDevice(), &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
        throw std::runtime_error("failed to create shader module!");
    }
    return shaderModule;
}

void GraphicsPipeline::SetShaderModules(std::vector<ShaderModule> &&rrModules) {
    shaderModules_ = std::move(rrModules);
    shaderModulesDirty_ = true;
}

void GraphicsPipeline::SetDescriptorSetBinding(std::vector<VkDescriptorSetLayoutBinding> &&rrBindings) {
    descriptorSetLayoutBindings_ = std::move(rrBindings);
    descriptorSetLayoutBindingDirty_ = true;
}

void GraphicsPipeline::SetVertexBindings(
    std::vector<VkVertexInputBindingDescription> &&rrInputBindingDescriptions,
    std::vector<VkVertexInputAttributeDescription> &&rrInputAttributeDescriptions) {
    inputBindingDescriptions_ = std::move(rrInputBindingDescriptions);
    inputAttributeDescriptions_ = std::move(rrInputAttributeDescriptions);
    inputBindingsDirty_ = true;
}

void GraphicsPipeline::Update() {
    if (shaderModulesDirty_ || descriptorSetLayoutBindingDirty_ || inputBindingsDirty_) {
        if (shaderModulesDirty_) {
            // Go through current shader modules and create
            destroyShaderModules();
            createShaderModules();
            shaderModulesDirty_ = false;
        }

        /*
        // Create descriptor set layout
        VkDescriptorSetLayoutBinding uboLayoutBinding{};
        uboLayoutBinding.binding = 0;
        uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uboLayoutBinding.descriptorCount = 1;
        uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        uboLayoutBinding.pImmutableSamplers = nullptr;  // Optional
        */

        VkDescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = static_cast<uint32_t>(descriptorSetLayoutBindings_.size());
        layoutInfo.pBindings = descriptorSetLayoutBindings_.data();

        // Needs to be destroyed
        if (vkCreateDescriptorSetLayout(deviceContext_.GetDevice(), &layoutInfo, nullptr, &descriptorSetLayout_) !=
            VK_SUCCESS) {
            throw std::runtime_error("failed to create descriptor set layout!");
        }

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 1;                   // Optional
        pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout_;  // Optional
        pipelineLayoutInfo.pushConstantRangeCount = 0;           // Optional
        pipelineLayoutInfo.pPushConstantRanges = nullptr;        // Optional

        if (vkCreatePipelineLayout(deviceContext_.GetDevice(), &pipelineLayoutInfo, nullptr, &pipelineLayout_) !=
            VK_SUCCESS) {
            throw std::runtime_error("failed to create pipeline layout!");
        }

        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(inputBindingDescriptions_.size());
        vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(inputAttributeDescriptions_.size());
        vertexInputInfo.pVertexBindingDescriptions = inputBindingDescriptions_.data();
        vertexInputInfo.pVertexAttributeDescriptions = inputAttributeDescriptions_.data();

        // Setup geometry type
        VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputAssembly.primitiveRestartEnable = VK_FALSE;

        // Where to draw
        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(extent_.width);
        viewport.height = static_cast<float>(extent_.height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        // Which pixels should be drawn (anything outside gets discarded)
        VkRect2D scissor{};
        scissor.offset = {0, 0};
        scissor.extent = extent_;

        // Compile both into viewport state
        VkPipelineViewportStateCreateInfo viewportState{};
        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount = 1;
        viewportState.pViewports = &viewport;
        viewportState.scissorCount = 1;
        viewportState.pScissors = &scissor;

        // Rasterizer takes geometry from vertex shader and turns it into fragments for fragment shader
        VkPipelineRasterizationStateCreateInfo rasterizer{};
        rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.depthClampEnable = VK_FALSE;
        rasterizer.rasterizerDiscardEnable = VK_FALSE;
        rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizer.lineWidth = 1.0f;
        rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
        rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
        rasterizer.depthBiasEnable = VK_FALSE;
        rasterizer.depthBiasConstantFactor = 0.0f;  // Optional
        rasterizer.depthBiasClamp = 0.0f;           // Optional
        rasterizer.depthBiasSlopeFactor = 0.0f;     // Optional

        // Multisampling
        VkPipelineMultisampleStateCreateInfo multisampling{};
        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        multisampling.minSampleShading = 1.0f;           // Optional
        multisampling.pSampleMask = nullptr;             // Optional
        multisampling.alphaToCoverageEnable = VK_FALSE;  // Optional
        multisampling.alphaToOneEnable = VK_FALSE;       // Optional

        // Color blending
        VkPipelineColorBlendAttachmentState colorBlendAttachment{};
        colorBlendAttachment.colorWriteMask =
            VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachment.blendEnable = VK_FALSE;
        colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;   // Optional
        colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;  // Optional
        colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;              // Optional
        colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;   // Optional
        colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;  // Optional
        colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;              // Optional

        VkPipelineColorBlendStateCreateInfo colorBlending{};
        colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.logicOp = VK_LOGIC_OP_COPY;  // Optional
        colorBlending.attachmentCount = 1;
        colorBlending.pAttachments = &colorBlendAttachment;
        colorBlending.blendConstants[0] = 0.0f;  // Optional
        colorBlending.blendConstants[1] = 0.0f;  // Optional
        colorBlending.blendConstants[2] = 0.0f;  // Optional
        colorBlending.blendConstants[3] = 0.0f;  // Optional

        VkGraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.stageCount = static_cast<uint32_t>(shaderStages_.size());
        pipelineInfo.pStages = shaderStages_.data();
        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &inputAssembly;
        pipelineInfo.pViewportState = &viewportState;
        pipelineInfo.pRasterizationState = &rasterizer;
        pipelineInfo.pMultisampleState = &multisampling;
        pipelineInfo.pDepthStencilState = nullptr;  // Optional
        pipelineInfo.pColorBlendState = &colorBlending;
        pipelineInfo.pDynamicState = nullptr;  // Optional
        pipelineInfo.layout = pipelineLayout_;
        pipelineInfo.renderPass = renderPass_;
        pipelineInfo.subpass = 0;
        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;  // Optional
        pipelineInfo.basePipelineIndex = -1;               // Optional

        if (vkCreateGraphicsPipelines(deviceContext_.GetDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr,
                                      &pipeline_) != VK_SUCCESS) {
            throw std::runtime_error("failed to create graphics pipeline!");
        }

        descriptorSetLayoutBindingDirty_ = false;
        inputBindingsDirty_ = false;
    }

    shaderModulesDirty_ = false;
}

void GraphicsPipeline::Bind(VkCommandBuffer &rCommandBuffer) {
    // Bind graphics pipeline
    vkCmdBindPipeline(rCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_);

    // vkCmdBindDescriptorSets(rCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelineLayout, 0, 1,
    // &m_descriptorSets[i], 0, nullptr);
}

void GraphicsPipeline::destroyShaderModules() {
    for (VkPipelineShaderStageCreateInfo &rInfo : shaderStages_) {
        // destroy old shader modules
        vkDestroyShaderModule(deviceContext_.GetDevice(), rInfo.module, nullptr);
    }
    shaderStages_.clear();
}

void GraphicsPipeline::createShaderModules() {
    assert(shaderStages_.empty());
    for (const ShaderModule &rModule : shaderModules_) {
        VkShaderModule shaderModule = createShaderModule(rModule.Code);

        VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
        vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertShaderStageInfo.stage = rModule.Flags;
        vertShaderStageInfo.module = shaderModule;
        vertShaderStageInfo.pName = "main";

        shaderStages_.push_back(std::move(vertShaderStageInfo));
    }
}
