//========================================================================
// OVRenderer (Open Vulkan Renderer) 
// Version: 0.1 
//------------------------------------------------------------------------
// Copyright (c) 2022-2022 Nagornov Vladimir <vladimirnagornov831@gmail.com>
//========================================================================
#pragma once
#include "ovr_device.h"
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <vector>
#include <memory>

namespace ovr {
	class OvrImage {
	public:
		
		void OvrImage::loadImage(const std::string& filepath);

		OvrImage(OVRDevice& device);
		~OvrImage();

		OvrImage(const OvrImage&) = delete;
		OvrImage& operator=(const OvrImage&) = delete;

		void bind(VkCommandBuffer commandBuffer);
		void draw(VkCommandBuffer commandBuffer);

		static std::unique_ptr<OvrImage> createImageFromFile(
			OVRDevice& device, const std::string& filepath);

		struct Builder {

			void loadImage(const std::string& filepath);
		};
	private:

		OVRDevice& ovrDevice;

		VkBuffer vertexBuffer;
		VkDeviceMemory vertexBufferMemory;
		uint32_t vertexCount;

		bool hasIndexBuffer = false;

		VkBuffer indexBuffer;
		VkDeviceMemory indexBufferMemory;
		uint32_t indexCount;
	};
}