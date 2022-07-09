#pragma once

#include <cstddef>
#include <vector>
#include "DeviceContext.h"
#include "Swapchain.h"

class GraphicsPipeline {
public:
    struct ShaderModule {
        VkShaderStageFlagBits Flags;
        std::vector<char> Code;
    };

public:
    GraphicsPipeline(DeviceContext &rDeviceContext, Swapchain &rSwapchain, VkRenderPass renderPass,
                     VkFormat imageFormat);
    virtual ~GraphicsPipeline();
    VkShaderModule createShaderModule(const std::vector<char> &rCode);

    void SetShaderModules(std::vector<ShaderModule> &&rrModules);

    void SetDescriptorSetBinding(std::vector<VkDescriptorSetLayoutBinding> &&rrBindings);

    void SetVertexBindings(std::vector<VkVertexInputBindingDescription> &&rrInputBindingDescriptions,
                           std::vector<VkVertexInputAttributeDescription> &&rrInputAttributeDescriptions);

    void Update();
    void Bind(VkCommandBuffer &rCommandBuffer);

private:
    void destroyShaderModules();
    void createShaderModules();

private:
    DeviceContext &deviceContext_;
    VkRenderPass renderPass_;
    VkExtent2D extent_;

    std::vector<VkPipelineShaderStageCreateInfo> shaderStages_;
    std::vector<ShaderModule> shaderModules_;
    bool shaderModulesDirty_ = false;

    std::vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindings_;
    bool descriptorSetLayoutBindingDirty_ = false;

    std::vector<VkVertexInputBindingDescription> inputBindingDescriptions_;
    std::vector<VkVertexInputAttributeDescription> inputAttributeDescriptions_;
    bool inputBindingsDirty_ = false;

    VkDescriptorSetLayout descriptorSetLayout_ = VK_NULL_HANDLE;
    VkPipelineLayout pipelineLayout_ = VK_NULL_HANDLE;
    VkPipeline pipeline_ = VK_NULL_HANDLE;
};
