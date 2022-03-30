#include "Engine.h"

#include "DeviceContext.h"
#include "Window.h"

Engine::Engine(DeviceContext &rContext, Window &rWindow)
    : rDeviceContext_(rContext), rWindow_(rWindow), swapchain_(rContext, rWindow) {
    commandPool_ = createCommandPool(swapchain_.GetSurface());
}

Engine::~Engine() {
    m_rContext.WaitIdle();
    destroyCommandPool();
    destroyFramebuffers();
    destroyRenderPass(renderPass_);
}

void Engine::Render() {
    // First update swapchain
    bool outOfDate = swapchain_.Update();

    if (outOfDate) {
        // Recreate command buffers only if number of swap chain images changed (and on initial render)
        if(commandBuffers_.size() != swapchain_.GetNumberOfImages()) {
            commandBuffers_ = createCommandBuffers(swapchain_.GetNumberOfImages(), commandPool_);
        }

        // Frame buffers are always destroyed when swapchain is out of date (change of images)
        destroyFramebuffers();

        if (renderPass_.imageFormat_ != swapchain_.GetImageFormat()) {
            // Only destroy render pass if image format no longer matches    
            destroyRenderPass(renderPass_);
        }

        if (renderPass_.renderPass_ == VK_NULL_HANDLE) {
            // Recreate render pass
            renderPass_ = createRenderPass(swapchain_.GetImageFormat());
        }

        // Recreate frame buffers
        swapchainFramebuffers_ = createFramebuffers(swapchain_, renderPass_);

        // Recreate pipeline
        spPipeline_ = std::make_unique<GraphicsPipeline>(renderPass_);

        // Record all command buffers, bind pipeline etc.

        // Framebuffers represent a collection of memory attachments

        // Do we need to update attachements etc also? -> swapchain_.Update could return true if resize is needed
        // In this case we would also resize attachements etc, but how?

        // First we need to gather any objects we want to render and put them into correct RenderPass -> Pipeline ->
        // Materials -> Mesh

        // May need to read up on RenderPass vs SubPass, why can't I do everything with sub passes?

        // Ok, from what I gathered: SubPass is limited in that they cannot access pixels other than the current one (e.g no
        // blur effects)

        // As I currently don't need that, we start off with one RenderPass, that may have multiple sub passes based on
        // inputs, is this easily doable? Maybe? But not sure, what does Unity have

        // https://docs.unity3d.com/Manual/shader-predefined-pass-tags-built-in.html
    }

    swapchain_.WaitForNextFrame();
    Swapchain::AvailableImageInfo availableInfo = swapchain_.AcquireImage();
    if(!availableInfo.IsValid())
    {
        outOfDate = true;
        return;
    }

    // At this point we know which commandBuffer we can use to record

    update(availableInfo);
    submit(availableInfo);
    if(!swapchain_.Present(availableInfo))
    {
        outOfDate = true;
        return;
    }
}

void Engine::update(const Swapchain::AvailableImageInfo &availableInfo)
{
	static auto startTime = std::chrono::high_resolution_clock::now();

	auto currentTime = std::chrono::high_resolution_clock::now();
	float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
	
	UniformBufferObject ubo{};
	ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.proj = glm::perspective(glm::radians(45.0f), m_rSwapchain.GetExtent2D().width / (float)m_rSwapchain.GetExtent2D().height, 0.1f, 10.0f);
	ubo.proj[1][1] *= -1;
	void *data;
	vkMapMemory(m_rDeviceContext.GetDevice(), m_uniformBuffersMemory[availableInfo.imageIndex], 0, sizeof(ubo), 0, &data);
	memcpy(data, &ubo, sizeof(ubo));
	vkUnmapMemory(m_rDeviceContext.GetDevice(), m_uniformBuffersMemory[availableInfo.imageIndex]);
}

void Engine::submit(const Swapchain::AvailableImageInfo &availableInfo)
{
	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = {availableInfo.imageAvailableSemaphore};
	VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;

	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffers_[availableInfo.imageIndex];

	VkSemaphore signalSemaphores[] = {availableInfo.renderFinishedSemaphore};
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	rDeviceContext_.Submit(std::move(submitInfo), availableInfo.inFlightFence);
}

VkCommandPool Engine::createCommandPool(VkSurfaceKHR surface) {
    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = rDeviceContext_.GetQueueFamilyIndices().graphicsFamily.value();
    poolInfo.flags = 0;  // Optional

    VkCommandPool commandPool;
    if (vkCreateCommandPool(rDeviceContext_.GetDevice(), &poolInfo, nullptr, &commandPool) != VK_SUCCESS) {
        throw std::runtime_error("failed to create command pool!");
    }

    return commandPool;
}

void Engine::destroyCommandPool() {
    // Buffers will be automatically freed on destruction of pool
    commandBuffers_.clear();

	if(commandPool_ != VK_NULL_HANDLE)
	{
		vkDestroyCommandPool(rDeviceContext_.GetDevice(), commandPool_, nullptr);
		commandPool_ = VK_NULL_HANDLE;
	}
}

std::vector<VkCommandBuffer> Engine::createCommandBuffers(uint32_t numImages, VkCommandPool &rPool) {
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = rPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = numImages;

    std::vector<VkCommandBuffer> commandBuffers;
    commandBuffers.resize(numImages);

    // Command buffers will be automatically freed
    if (vkAllocateCommandBuffers(rDeviceContext_.GetDevice(), &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate command buffers!");
    }
    return commandBuffers;
}

Engine::RenderPass Engine::createRenderPass(const VkFormat &swapchainImageFormat) {
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = swapchainImageFormat;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;

    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &colorAttachment;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    VkRenderPass renderPass;
    if (vkCreateRenderPass(rDeviceContext_.GetDevice(), &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS) {
        throw std::runtime_error("failed to create render pass!");
    }
    return RenderPass{renderPass, swapchainImageFormat};
}

void Engine::destroyRenderPass(RenderPass &rRenderPass) {
    if (rRenderPass.renderPass_ != VK_NULL_HANDLE) {
        vkDestroyRenderPass(rDeviceContext_.GetDevice(), rRenderPass.renderPass_, nullptr);
        rRenderPass.renderPass_ = VK_NULL_HANDLE;
    }
}

std::vector<VkFramebuffer> Engine::createFramebuffers(Swapchain &rSwapchain, RenderPass &rRenderPass) {
    std::vector<VkFramebuffer> framebuffers;
    framebuffers.resize(rSwapchain.GetImageViews().size());

    for (size_t i = 0; i < rSwapchain.GetImageViews().size(); i++) {
        VkImageView attachments[] = {rSwapchain.GetImageViews()[i]};

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = rRenderPass.renderPass_;
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments = attachments;
        framebufferInfo.width = rSwapchain.GetExtent2D().width;
        framebufferInfo.height = rSwapchain.GetExtent2D().height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(rDeviceContext_.GetDevice(), &framebufferInfo, nullptr, &framebuffers[i]) !=
            VK_SUCCESS) {
            throw std::runtime_error("failed to create framebuffer!");
        }
    }

    return framebuffers;
}

void Engine::destroyFramebuffers() {
    for (auto framebuffer : swapchainFramebuffers_) {
        vkDestroyFramebuffer(rDeviceContext_.GetDevice(), framebuffer, nullptr);
    }
    swapchainFramebuffers_.clear();
}
