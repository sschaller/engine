#pragma once

#include <stdint.h>
#include <vector>

#include "DeviceContext.h"

class Swapchain final {
public:
    struct SwapChainSupportDetails {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;

        bool IsAdequate() { return !formats.empty() && !presentModes.empty(); }
    };

    struct AvailableImageInfo {
        uint32_t imageIndex = std::numeric_limits<uint32_t>::max();
        VkSemaphore imageAvailableSemaphore = VK_NULL_HANDLE;
        VkSemaphore renderFinishedSemaphore = VK_NULL_HANDLE;
        VkFence inFlightFence = VK_NULL_HANDLE;

        bool IsValid() { return imageIndex != std::numeric_limits<uint32_t>::max(); }
    };

public:
    Swapchain(DeviceContext &rDeviceContext, Window &rWindow);
    ~Swapchain();

    bool Update();

    void DestroyResources();

	void WaitForNextFrame();

	AvailableImageInfo AcquireImage();

	bool Present(const AvailableImageInfo &availableInfo);

    VkFormat &GetImageFormat() { return swapchainImageFormat_; }
	VkExtent2D &GetExtent2D() { return swapchainExtent_; }
	VkSurfaceKHR &GetSurface() { return surface_; }
	std::vector<VkImageView> &GetImageViews() { return swapchainImageViews_; }
    uint32_t GetNumberOfImages() const { return static_cast<uint32_t>(swapchainImages_.size()); }

public:
    static SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);

private:
	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);

	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes);

	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities, uint32_t width, uint32_t height);

	void createSyncObjects();

	void cleanupSyncObjects();

private:
    DeviceContext &rDeviceContext_;
    Window &rWindow_;
    bool outOfDate_ = true;
    VkSurfaceKHR surface_ = VK_NULL_HANDLE;
    VkSwapchainKHR swapchain_ = VK_NULL_HANDLE;
    std::vector<VkImage> swapchainImages_;
    std::vector<VkImageView> swapchainImageViews_;
    VkFormat swapchainImageFormat_ = VK_FORMAT_UNDEFINED;
    VkExtent2D swapchainExtent_{};

	// Synchronisation on GPU
	std::vector<VkSemaphore> imageAvailableSemaphores_;
	std::vector<VkSemaphore> renderFinishedSemaphores_;

	// Synchronisation GPU-CPU
	std::vector<VkFence> inFlightFences_;

	// Mapping of Image -> InFlightFence assigned to this image (or VK_NULL_HANDLE)
	std::vector<VkFence> imagesInFlight_;

	uint32_t m_currentFrame = 0;
};
