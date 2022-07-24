#pragma once
#include <vulkan/vulkan.h>

#include <functional>
#include <optional>
#include "ResourceManager.h"

class Window;

class DeviceContext final {
public:
    struct QueueFamilyIndices {
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;

        bool isComplete() { return graphicsFamily.has_value() && presentFamily.has_value(); }
    };

public:
    DeviceContext(const std::vector<const char *> &requiredExtensions);
    ~DeviceContext();
    
    void CreateDevice(VkSurfaceKHR &rSurface);

	void Submit(VkSubmitInfo &&submitInfo, const VkFence &buffersReadyFence);

	VkResult Present(const VkPresentInfoKHR &presentInfo);

    void WaitIdle();
    
    VkInstance &GetInstance() { return instance_; }
    VkPhysicalDevice &GetPhysicalDevice() { return physicalDevice_; }
    VkDevice &GetDevice() { return device_; }
    const QueueFamilyIndices &GetQueueFamilyIndices() const { return queueFamilyIndices_; }

public:
	static QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface);

private:
    ResourceManager resourceManager_;
    VkInstance instance_ = VK_NULL_HANDLE;
    VkDebugUtilsMessengerEXT debugMessenger_ = VK_NULL_HANDLE;
    VkPhysicalDevice physicalDevice_ = VK_NULL_HANDLE;
    VkDevice device_ = VK_NULL_HANDLE;
	VkQueue graphicsQueue_ = VK_NULL_HANDLE;
	VkQueue presentQueue_ = VK_NULL_HANDLE;
    QueueFamilyIndices queueFamilyIndices_;
};