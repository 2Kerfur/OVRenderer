//========================================================================
// OVRenderer (Open Vulkan Renderer) 
// Version: 0.1 
//------------------------------------------------------------------------
// Copyright (c) 2022-2022 Nagornov Vladimir <vladimirnagornov831@gmail.com>
//========================================================================


#pragma once

#include "AppWindow.h"
#include "ovr_device.h"
#include "ovr_swap_chain.h"

#include <memory>
#include <vector>
#include <cassert>

namespace ovr {

	class OvrRenderer {

	public:

		OvrRenderer(AppWindow& window, OVRDevice &device);
		~OvrRenderer();

		// c++11 Disallow copying (compiler will not generate those constructors)
		OvrRenderer(const OvrRenderer&) = delete;
		OvrRenderer& operator=(const OvrRenderer&) = delete;

		VkRenderPass getSwapChainRenderPass() const { return ovrSwapChain->getRenderPass(); }
		float getAspectRatio() const { return ovrSwapChain->extentAspectRatio(); }
		bool isFrameInProgress() const { return isFrameStarted; }

		VkCommandBuffer getCurrentCommandBuffer() const {
			assert(isFrameStarted && "Cannot get command buffer when fram not in progress");
			return commandBuffers[currentFrameIndex];
		}

		int GetFrameIndex() const {
			assert(isFrameStarted && "Cannot get frame index when frame not in progress");
			return currentFrameIndex;
		}

		VkCommandBuffer beginFrame();
		void endFrame();
		void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
		void endSwapChainRenderPass(VkCommandBuffer commandBuffer);


	private:
		void createCommandBuffers();
		void freeCommandBuffers();
		void recreateSwapChain();


		AppWindow& appWindow;
		OVRDevice& ovrDevice;
		std::unique_ptr<OVRSwapChain> ovrSwapChain;
		std::vector<VkCommandBuffer> commandBuffers;

		uint32_t currentImageIndex;
		int currentFrameIndex{ 0 };
		bool isFrameStarted{false};
	};
}