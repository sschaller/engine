#pragma once

#include <memory>
#include <vector>

#include "GraphicsPipeline.h"
#include "Swapchain.h"

class DeviceContext;
class Scene;
class Window;

class Engine {
public:
    Engine(Scene &rScene, DeviceContext &rContext, Window &rWindow);
    ~Engine();

    void Render();

private:
    void update(const Swapchain::AvailableImageInfo &availableInfo, bool outOfDate);
    void submit(const Swapchain::AvailableImageInfo &availableInfo);

    VkCommandPool createCommandPool(VkSurfaceKHR surface);
    void destroyCommandPool();
    std::vector<VkCommandBuffer> createCommandBuffers(uint32_t numImages, VkCommandPool &rPool);
    VkRenderPass createRenderPass(const VkFormat &swapchainImageFormat);
    void destroyRenderPass(VkRenderPass &rRenderPass);
    std::vector<VkFramebuffer> createFramebuffers(Swapchain &rSwapchain, VkRenderPass &rRenderPass);
    void destroyFramebuffers();

private:
    Scene &rScene_;
    DeviceContext &rDeviceContext_;
    Window &rWindow_;
    Swapchain swapchain_;
    VkRenderPass renderPass_ = VK_NULL_HANDLE;
    VkFormat imageFormat_ = VK_FORMAT_UNDEFINED;
    VkCommandPool commandPool_ = VK_NULL_HANDLE;
    std::vector<VkCommandBuffer> commandBuffers_;
    std::vector<VkFramebuffer> swapchainFramebuffers_;
};
