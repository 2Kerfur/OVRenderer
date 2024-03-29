//========================================================================
// OVRenderer (Open Vulkan Renderer) 
// Version: 0.1 
//------------------------------------------------------------------------
// Copyright (c) 2022-2022 Nagornov Vladimir <vladimirnagornov831@gmail.com>
//========================================================================
#include "ovr_renderer.h"



#include <array>
#include <stdexcept>
#include <array>

namespace ovr {

	OvrRenderer::OvrRenderer(AppWindow& window, OVRDevice& device) : 
		appWindow{ window }, ovrDevice{ device } {
		recreateSwapChain();
		createCommandBuffers();
	}

	OvrRenderer::~OvrRenderer() {
		freeCommandBuffers();
	}

	void OvrRenderer::recreateSwapChain() {
		auto extent = appWindow.getExtent();
		while (extent.width == 0 || extent.height == 0) {
			extent = appWindow.getExtent();
			glfwWaitEvents();
		}

		vkDeviceWaitIdle(ovrDevice.device());

		if (ovrSwapChain == nullptr) {
			ovrSwapChain = nullptr;
			ovrSwapChain = std::make_unique<OVRSwapChain>(ovrDevice, extent);
		}
		else {
			std::shared_ptr<OVRSwapChain> oldSwapChain = std::move(ovrSwapChain);
			ovrSwapChain = std::make_unique<OVRSwapChain>(ovrDevice, extent, oldSwapChain);

			if (!oldSwapChain->compareSwapFormats(*ovrSwapChain.get())) {
				throw std::runtime_error("Swap chain image(or depth) format has changed!");
			}

		}

	}

	void OvrRenderer::createCommandBuffers()
	{
		commandBuffers.resize(OVRSwapChain::MAX_FRAMES_IN_FLIGHT);

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = ovrDevice.getCommandPool();
		allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

		if (vkAllocateCommandBuffers(ovrDevice.device(),
			&allocInfo,
			commandBuffers.data()) !=
			VK_SUCCESS) {
			throw std::runtime_error("failed to allocate command buffers!");
		}


	}
	void OvrRenderer::freeCommandBuffers() {
		vkFreeCommandBuffers(ovrDevice.device(),
			ovrDevice.getCommandPool(),
			static_cast<uint32_t>(commandBuffers.size()),
			commandBuffers.data());
		commandBuffers.clear();
	}


	VkCommandBuffer OvrRenderer::beginFrame()
	{
		assert(!isFrameStarted && "Can't call beginFrame while already in progress");
		
		auto result = ovrSwapChain->acquireNextImage(&currentImageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR) {
			recreateSwapChain();
			return nullptr;
		}

		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			throw std::runtime_error("failed to acquire swap chain image!");

		}

		isFrameStarted = true;
		auto commandBuffer = getCurrentCommandBuffer();

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("failed to begin recording command buffer");
		}
		return commandBuffer;
	}
	void OvrRenderer::endFrame()
	{
		assert(isFrameStarted && "Can't call endFrame while frame is not in progress");
		auto commandBuffer = getCurrentCommandBuffer();

		if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
			throw std::runtime_error("failed to record command buffer!");
		}

		auto result = ovrSwapChain->submitCommandBuffers(&commandBuffer, &currentImageIndex);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR ||
			appWindow.wasWindowResized()) {
			appWindow.resetWindowResizedFlag();
			recreateSwapChain();
		}
		else if (result != VK_SUCCESS) {
			throw std::runtime_error("failed to present swap chain image!");

		}

		isFrameStarted = false;
		currentFrameIndex = (currentFrameIndex + 1) % OVRSwapChain::MAX_FRAMES_IN_FLIGHT;
	}

	void OvrRenderer::beginSwapChainRenderPass(VkCommandBuffer commandBuffer)
	{
		assert(isFrameStarted && "Can't call beginSwapChainRenderPass if frame is not in progress");
		assert(
			commandBuffer == getCurrentCommandBuffer() &&
			"Can't begin render pass on command buffer from a differnt frame");
	
		/*VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(commandBuffers[currentImageIndex], &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("failed to begin recording command buffer");
		}*/

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = ovrSwapChain->getRenderPass();
		renderPassInfo.framebuffer = ovrSwapChain->getFrameBuffer(currentImageIndex);

		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = ovrSwapChain->getSwapChainExtent();

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { 0.1f, 0.3f, 0.1f, 1.0f };
		clearValues[1].depthStencil = { 1.0f, 0 };
		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());

		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(ovrSwapChain->getSwapChainExtent().width);
		viewport.height = static_cast<float>(ovrSwapChain->getSwapChainExtent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		VkRect2D scissor{ {0, 0}, ovrSwapChain->getSwapChainExtent() };
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

	}

	void OvrRenderer::endSwapChainRenderPass(VkCommandBuffer commandBuffer)
	{
		assert(isFrameStarted && "Can't call endSwapChainRenderPass if frame is not in progress");
		assert(
			commandBuffer == getCurrentCommandBuffer() &&
			"Can't end render pass on command buffer from a differnt frame");
		vkCmdEndRenderPass(commandBuffer);
	}
}