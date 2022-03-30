#include "DeviceContext.h"

#include <iostream>
#include <set>
#include <stdexcept>
#include <vector>

#include "Swapchain.h"
#include "Window.h"

namespace {
const std::vector<const char *> validationLayers = {"VK_LAYER_KHRONOS_validation"};

const std::vector<const char *> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                             VkDebugUtilsMessageTypeFlagsEXT messageType,
                                             const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                                             void *pUserData) {
    std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

    return VK_FALSE;
}

VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
                                      const VkAllocationCallbacks *pAllocator,
                                      VkDebugUtilsMessengerEXT *pDebugMessenger) {
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger,
                                   const VkAllocationCallbacks *pAllocator) {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, debugMessenger, pAllocator);
    }
}

bool checkValidationLayerSupport() {
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char *layerName : validationLayers) {
        bool layerFound = false;

        for (const auto &layerProperties : availableLayers) {
            if (strcmp(layerName, layerProperties.layerName) == 0) {
                layerFound = true;
                break;
            }
        }

        if (!layerFound) {
            return false;
        }
    }

    return true;
}

void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo) {
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                                 VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                 VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                             VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                             VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;
}

VkInstance createInstance(const std::vector<const char *> &requiredExtensions) {
    if (enableValidationLayers && !checkValidationLayerSupport()) {
        throw std::runtime_error("validation layers requested, but not available!");
    }

    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Application";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    std::vector<const char *> extensions(requiredExtensions);
    if (enableValidationLayers) {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
    if (enableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();

        populateDebugMessengerCreateInfo(debugCreateInfo);
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT *)&debugCreateInfo;
    } else {
        createInfo.enabledLayerCount = 0;

        createInfo.pNext = nullptr;
    }

    VkInstance instance;
    if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
        throw std::runtime_error("failed to create instance!");
    }
    return instance;
}

VkDebugUtilsMessengerEXT setupDebugMessenger(VkInstance instance) {
    if (!enableValidationLayers) {
        return VK_NULL_HANDLE;
    }

    VkDebugUtilsMessengerCreateInfoEXT createInfo{};
    populateDebugMessengerCreateInfo(createInfo);

    VkDebugUtilsMessengerEXT debugMessenger;
    if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
        throw std::runtime_error("failed to set up debug messenger!");
    }
    return debugMessenger;
}

bool checkDeviceExtensionSupport(VkPhysicalDevice physicalDevice) {
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, availableExtensions.data());

    std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

    for (const auto &extension : availableExtensions) {
        requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
}

bool isDeviceSuitable(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface) {
    DeviceContext::QueueFamilyIndices indices = DeviceContext::FindQueueFamilies(physicalDevice, surface);

    bool extensionsSupported = checkDeviceExtensionSupport(physicalDevice);

    bool swapchainAdequate = false;
    if (extensionsSupported) {
        // only query if extensions are supported
        swapchainAdequate = Swapchain::QuerySwapChainSupport(physicalDevice, surface).IsAdequate();
    }

    return indices.isComplete() && extensionsSupported;
}

VkPhysicalDevice pickPhysicalDevice(VkInstance instance, VkSurfaceKHR surface) {
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

    if (deviceCount == 0) {
        throw std::runtime_error("failed to find GPUs with Vulkan support!");
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

    for (const VkPhysicalDevice &device : devices) {
        if (isDeviceSuitable(device, surface)) {
            return device;
        }
    }
    throw std::runtime_error("failed to find a suitable GPU!");
}

VkDevice createLogicalDevice(VkPhysicalDevice physicalDevice, const DeviceContext::QueueFamilyIndices &rIndices) {
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = {rIndices.graphicsFamily.value(), rIndices.presentFamily.value()};

    float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies) {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures{};

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pQueueCreateInfos = queueCreateInfos.data();

    createInfo.pEnabledFeatures = &deviceFeatures;

    createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
    createInfo.ppEnabledExtensionNames = deviceExtensions.data();

    if (enableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
    } else {
        createInfo.enabledLayerCount = 0;
    }

    VkDevice device;
    if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS) {
        throw std::runtime_error("failed to create logical device!");
    }
    return device;
}

}  // namespace

DeviceContext::DeviceContext(const std::vector<const char *> &requiredExtensions) {
    instance_ = createInstance(requiredExtensions);
    debugMessenger_ = setupDebugMessenger(instance_);
}

DeviceContext::~DeviceContext() {
    if (device_ != VK_NULL_HANDLE) {
        vkDestroyDevice(device_, nullptr);
    }

    if (enableValidationLayers) {
        DestroyDebugUtilsMessengerEXT(instance_, debugMessenger_, nullptr);
    }
    vkDestroyInstance(instance_, nullptr);
}

void DeviceContext::CreateDevice(VkSurfaceKHR &rSurface) {
    if (device_ != VK_NULL_HANDLE) {
        // Device was already created
        return;
    }

    physicalDevice_ = pickPhysicalDevice(instance_, rSurface);
    queueFamilyIndices_ = FindQueueFamilies(physicalDevice_, rSurface);

    device_ = createLogicalDevice(physicalDevice_, queueFamilyIndices_);

    vkGetDeviceQueue(device_, queueFamilyIndices_.graphicsFamily.value(), 0, &graphicsQueue_);
    vkGetDeviceQueue(device_, queueFamilyIndices_.presentFamily.value(), 0, &presentQueue_);
}

void DeviceContext::Submit(VkSubmitInfo &&submitInfo, const VkFence &buffersReadyFence)
{
	if(buffersReadyFence != VK_NULL_HANDLE)
	{
		vkResetFences(device_, 1, &buffersReadyFence);
	}
	VkResult result = vkQueueSubmit(graphicsQueue_, 1, &submitInfo, buffersReadyFence);
	if(result != VK_SUCCESS)
	{
		throw std::runtime_error("failed to submit draw command buffer!");
	}
}


VkResult DeviceContext::Present(const VkPresentInfoKHR &presentInfo)
{
	return vkQueuePresentKHR(presentQueue_, &presentInfo);
}

void DeviceContext::WaitIdle()
{
	vkDeviceWaitIdle(device_);
}

DeviceContext::QueueFamilyIndices DeviceContext::FindQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface)
{
	QueueFamilyIndices indices;

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

	int i = 0;
	for(const auto &queueFamily : queueFamilies)
	{
		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

		if(queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			indices.graphicsFamily = i;
		}
		if(presentSupport)
		{
			indices.presentFamily = i;
		}
		if(indices.isComplete())
		{
			break;
		}
		i++;
	}

	return indices;
}