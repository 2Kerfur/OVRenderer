//========================================================================
// OVRenderer (Open Vulkan Renderer) 
// Version: 0.1 
//------------------------------------------------------------------------
// Copyright (c) 2022-2022 Nagornov Vladimir <vladimirnagornov831@gmail.com>
//========================================================================


#pragma once
#include "ovr_camera.h"
#include "ovr_pipeline.h"
#include "ovr_device.h"
#include "ovr_game_object.h"

#include <memory>
#include <vector>

namespace ovr {

	class SimpleRenderSystem {

	public:

		SimpleRenderSystem(OVRDevice &device, VkRenderPass renderPass);
		~SimpleRenderSystem();

		// c++11 Disallow copying (compiler will not generate those constructors)
		SimpleRenderSystem(const SimpleRenderSystem&) = delete;
		SimpleRenderSystem& operator=(const SimpleRenderSystem&) = delete;

		void renderGameObjects(VkCommandBuffer commandBuffer, 
			std::vector<OvrGameObject>& gameObjects,
			const OvrCamera &camera);

	private:
		void createPipelineLayout();
		void createPipeline(VkRenderPass renderPass);
	
		OVRDevice &ovrDevice;

		std::unique_ptr<OvrPipeline> ovrPipeline;
		VkPipelineLayout pipelineLayout;
	};
}