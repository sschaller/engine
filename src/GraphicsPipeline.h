#pragma once

#include <vector>
#include <vulkan/vulkan.h>

class DeviceContext;
class Swapchain;

class GraphicsPipeline {
public:
    GraphicsPipeline(DeviceContext &rDeviceContext, Swapchain &rSwapchain, VkRenderPass renderPass, VkFormat imageFormat);
    virtual ~GraphicsPipeline();
    VkShaderModule createShaderModule(const std::vector<char> &code);
    void Bind(VkCommandBuffer &rCommandBuffer);

private:
    DeviceContext &deviceContext_;
    VkShaderModule vertShaderModule_ = VK_NULL_HANDLE;
    VkShaderModule fragShaderModule_ = VK_NULL_HANDLE;
    VkDescriptorSetLayout descriptorSetLayout_ = VK_NULL_HANDLE;
    VkPipelineLayout pipelineLayout_ = VK_NULL_HANDLE;
    VkPipeline pipeline_ = VK_NULL_HANDLE;
};
