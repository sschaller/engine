#include "Swapchain.h"

#include <algorithm>

#include "Window.h"

namespace {
    const uint32_t MAX_FRAMES_IN_FLIGHT = 2u;
}

Swapchain::Swapchain(DeviceContext &rDeviceContext, Window &rWindow)
    : rDeviceContext_(rDeviceContext), rWindow_(rWindow) {
        surface_ = rWindow_.CreateSurface(rDeviceContext_);
        rDeviceContext_.CreateDevice(surface_);
    }

Swapchain::~Swapchain() {
    if(swapchain_ != VK_NULL_HANDLE) {
        DestroyResources();

		// Destroy Swapchain
		vkDestroySwapchainKHR(rDeviceContext_.GetDevice(), swapchain_, nullptr);
		swapchain_ = VK_NULL_HANDLE;
    }
    vkDestroySurfaceKHR(rDeviceContext_.GetInstance(), surface_, nullptr);
}

bool Swapchain::Update() {
    // Check if window resized

    if (!outOfDate_) {
        return false;
    }
    outOfDate_ = false;

    if (swapchain_ != VK_NULL_HANDLE) {
        // We already created a swapchain, first destroy resources of the existing one
        // Keeps swapchain around for reuse
        DestroyResources();
    }

    Window::Size windowSize = rWindow_.GetSize();
    SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(rDeviceContext_.GetPhysicalDevice(), surface_);

    VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
    swapchainImageFormat_ = surfaceFormat.format;
    VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
    swapchainExtent_ = chooseSwapExtent(swapChainSupport.capabilities, windowSize.width, windowSize.height);

    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
    if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = surface_;
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = swapchainImageFormat_;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = swapchainExtent_;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    DeviceContext::QueueFamilyIndices indices =
        DeviceContext::FindQueueFamilies(rDeviceContext_.GetPhysicalDevice(), surface_);
    uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};

    if (indices.graphicsFamily != indices.presentFamily) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    } else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0;      // Optional
        createInfo.pQueueFamilyIndices = nullptr;  // Optional
    }

    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = swapchain_;

    // Create Swapchain (Destroy required)
    if (vkCreateSwapchainKHR(rDeviceContext_.GetDevice(), &createInfo, nullptr, &swapchain_) != VK_SUCCESS) {
        throw std::runtime_error("failed to create swap chain!");
    }

    // Get Swapchain Images
    vkGetSwapchainImagesKHR(rDeviceContext_.GetDevice(), swapchain_, &imageCount, nullptr);
    swapchainImages_.resize(imageCount);
    vkGetSwapchainImagesKHR(rDeviceContext_.GetDevice(), swapchain_, &imageCount, swapchainImages_.data());

    // Create Image Views (Destroy required)
    swapchainImageViews_.resize(swapchainImages_.size());
    for (size_t i = 0; i < swapchainImages_.size(); i++) {
        VkImageViewCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = swapchainImages_[i];
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = swapchainImageFormat_;
        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;
        if (vkCreateImageView(rDeviceContext_.GetDevice(), &createInfo, nullptr, &swapchainImageViews_[i]) !=
            VK_SUCCESS) {
            throw std::runtime_error("failed to create image views!");
        }
    }

    createSyncObjects();

    return true;
}

/**
 * @brief Destroys resources of old swapchain.
 * Keeps swapchain around for reuse.
 */
void Swapchain::DestroyResources() {
    cleanupSyncObjects();

    // Destroy Image Views
    for(auto imageView : swapchainImageViews_)
    {
        vkDestroyImageView(rDeviceContext_.GetDevice(), imageView, nullptr);
    }
    swapchainImageViews_.clear();
}

void Swapchain::WaitForNextFrame()
{
	vkWaitForFences(rDeviceContext_.GetDevice(), 1, &inFlightFences_[m_currentFrame], VK_TRUE, UINT64_MAX);
}

Swapchain::AvailableImageInfo Swapchain::AcquireImage()
{
	uint32_t imageIndex;
	VkResult result = vkAcquireNextImageKHR(rDeviceContext_.GetDevice(), swapchain_, UINT64_MAX, imageAvailableSemaphores_[m_currentFrame], VK_NULL_HANDLE, &imageIndex);
	if(result == VK_ERROR_OUT_OF_DATE_KHR)
	{
		// Return available image info in error state
		return AvailableImageInfo{};
	}
	if(result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
	{
		throw std::runtime_error("Failed to acquire swap chain image!");
	}

	// Check if a previous frame is using this image (i.e. there is its fence to wait on)
	if(imagesInFlight_[imageIndex] != VK_NULL_HANDLE)
	{
		vkWaitForFences(rDeviceContext_.GetDevice(), 1, &imagesInFlight_[imageIndex], VK_TRUE, UINT64_MAX);
	}
	// Mark the image as now being in use by this frame
	imagesInFlight_[imageIndex] = inFlightFences_[m_currentFrame];

	AvailableImageInfo availableInfo;
	availableInfo.imageIndex = imageIndex;
	availableInfo.imageAvailableSemaphore = imageAvailableSemaphores_[m_currentFrame];
	availableInfo.renderFinishedSemaphore = renderFinishedSemaphores_[m_currentFrame];
	availableInfo.inFlightFence = inFlightFences_[m_currentFrame];

	return availableInfo;
}

bool Swapchain::Present(const AvailableImageInfo &availableInfo)
{
	VkSemaphore waitSemaphores[] = {availableInfo.renderFinishedSemaphore};

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = waitSemaphores;

	VkSwapchainKHR swapChains[] = {swapchain_};
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &availableInfo.imageIndex;
	presentInfo.pResults = nullptr; // Optional

	VkResult result = rDeviceContext_.Present(std::move(presentInfo));
	if(result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
	{
		return false;
	}
	if(result != VK_SUCCESS)
	{
		throw std::runtime_error("failed to present swap chain image!");
	}

	m_currentFrame = (m_currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
	return true;
}



Swapchain::SwapChainSupportDetails Swapchain::QuerySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface) {
    Swapchain::SwapChainSupportDetails details;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

    if (formatCount != 0) {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
    }

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

    if (presentModeCount != 0) {
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
    }

    return details;
}

VkSurfaceFormatKHR Swapchain::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats) {
    for (const auto &availableFormat : availableFormats) {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
            availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return availableFormat;
        }
    }

    return availableFormats[0];
}

VkPresentModeKHR Swapchain::chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes) {
    for (const auto &availablePresentMode : availablePresentModes) {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return availablePresentMode;
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D Swapchain::chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities, uint32_t width, uint32_t height) {
    if (capabilities.currentExtent.width != UINT32_MAX) {
        return capabilities.currentExtent;
    } else {
        VkExtent2D actualExtent = {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};

        actualExtent.width =
            std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actualExtent.height =
            std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

        return actualExtent;
    }
}

void Swapchain::createSyncObjects() {
    imageAvailableSemaphores_.resize(MAX_FRAMES_IN_FLIGHT);
    renderFinishedSemaphores_.resize(MAX_FRAMES_IN_FLIGHT);
    inFlightFences_.resize(MAX_FRAMES_IN_FLIGHT);
    imagesInFlight_.resize(swapchainImages_.size(), VK_NULL_HANDLE);

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        if (vkCreateSemaphore(rDeviceContext_.GetDevice(), &semaphoreInfo, nullptr, &imageAvailableSemaphores_[i]) !=
                VK_SUCCESS ||
            vkCreateSemaphore(rDeviceContext_.GetDevice(), &semaphoreInfo, nullptr, &renderFinishedSemaphores_[i]) !=
                VK_SUCCESS ||
            vkCreateFence(rDeviceContext_.GetDevice(), &fenceInfo, nullptr, &inFlightFences_[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create sync objects for a frame!");
        }
    }
}

void Swapchain::cleanupSyncObjects() {
    // Only contains references to fences in inFlightFences_.
    imagesInFlight_.clear();

    for (VkFence &fence : inFlightFences_) {
        vkDestroyFence(rDeviceContext_.GetDevice(), fence, nullptr);
    }
    inFlightFences_.clear();
    for (VkSemaphore &semaphore : renderFinishedSemaphores_) {
        vkDestroySemaphore(rDeviceContext_.GetDevice(), semaphore, nullptr);
    }
    renderFinishedSemaphores_.clear();

    for (VkSemaphore &semaphore : imageAvailableSemaphores_) {
        vkDestroySemaphore(rDeviceContext_.GetDevice(), semaphore, nullptr);
    }
    imageAvailableSemaphores_.clear();
}
