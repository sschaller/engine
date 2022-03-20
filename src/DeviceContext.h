#pragma once
#include <vulkan/vulkan.h>

#include <functional>


class DeviceContext final {
   public:
    DeviceContext(const std::vector<const char *> &requiredExtensions);
    ~DeviceContext();

   private:
    VkInstance instance_;
    VkDebugUtilsMessengerEXT debugMessenger_;
    VkPhysicalDevice physicalDevice_;
    VkDevice device_;
};