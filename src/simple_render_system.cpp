//========================================================================
// OVRenderer (Open Vulkan Renderer) 
// Version: 0.1 
//------------------------------------------------------------------------
// Copyright (c) 2022-2022 Nagornov Vladimir <vladimirnagornov831@gmail.com>
//========================================================================
#include "simple_render_system.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <array>
#include <stdexcept>
#include <array>



namespace ovr {
	struct SimplePushConstantData {
		glm::mat4 transform{ 1.f };
		glm::mat4 normalMatrix{ 1.f };
	};

	SimpleRenderSystem::SimpleRenderSystem(OVRDevice &device, VkRenderPass renderPass) : ovrDevice(device) {
		createPipelineLayout();
		createPipeline(renderPass);
	}

	SimpleRenderSystem::~SimpleRenderSystem() {
		vkDestroyPipelineLayout(ovrDevice.device(), pipelineLayout, nullptr);
	}

	
	void SimpleRenderSystem::createPipelineLayout()
	{
		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(SimplePushConstantData);


		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 0;
		pipelineLayoutInfo.pSetLayouts = nullptr;
		pipelineLayoutInfo.pushConstantRangeCount = 1;
		pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

		if (vkCreatePipelineLayout(ovrDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) !=
			VK_SUCCESS) {
			throw std::runtime_error("failed to create pipeline layout!");
		}
	}
	void SimpleRenderSystem::createPipeline(VkRenderPass renderPass)
	{
		assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");


		PipelineConfigInfo pipelineConfig{};
		OvrPipeline::defaultPipelineConfigInfo(
			pipelineConfig);

		pipelineConfig.renderPass = renderPass;
		pipelineConfig.pipelineLayout = pipelineLayout;
		ovrPipeline = std::make_unique<OvrPipeline>(
			ovrDevice,
			"resources/shaders/simple_shader.vert.spv",
			"resources/shaders/simple_shader.frag.spv",
			pipelineConfig
			);
	}

	void SimpleRenderSystem::renderGameObjects(VkCommandBuffer commandBuffer,
		std::vector<OvrGameObject>& gameObjects,
		const OvrCamera& camera) {

		
		ovrPipeline->bind(commandBuffer);

		auto projectionView = camera.getProjection() * camera.getView();

		for (auto& obj : gameObjects) {


			SimplePushConstantData push{};
			auto normalMatrix = obj.transform.mat4();
			push.transform = projectionView * normalMatrix;
			push.normalMatrix = normalMatrix;
			vkCmdPushConstants(
				commandBuffer,
				pipelineLayout,
				VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
				0,
				sizeof(SimplePushConstantData),
				&push);
			obj.model->bind(commandBuffer);
			obj.model->draw(commandBuffer);
		}
	}


}