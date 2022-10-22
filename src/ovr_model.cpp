//========================================================================
// OVRenderer (Open Vulkan Renderer) 
// Version: 0.1 
//------------------------------------------------------------------------
// Copyright (c) 2022-2022 Nagornov Vladimir <vladimirnagornov831@gmail.com>
//========================================================================
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include "ovr_model.h"

#include <iostream>

#include "ovr_utils.h"

#define GLM_ENABLE_EXPERIMENTAL

#include <cassert>
#include <cstring>
#include <glm/gtx/hash.hpp>
#include <unordered_map>

namespace std {
	template<> 
	struct hash<ovr::OvrModel::Vertex> {
		size_t operator()(ovr::OvrModel::Vertex const& vertex) const {
			size_t seed = 0;
			ovr::hashCombine(seed, vertex.position, vertex.color, vertex.normal, vertex.uv);
			return seed;
		}
	};
}

namespace ovr {
	OvrModel::OvrModel(OVRDevice& device, const OvrModel::Builder &builder) : ovrDevice{device}
	{
		CreateVertexBuffers(builder.vertices);
		CreateIndexBuffers(builder.indices);
	}

	OvrModel::~OvrModel()
	{
		vkDestroyBuffer(ovrDevice.device(), vertexBuffer, nullptr);
		vkFreeMemory(ovrDevice.device(), vertexBufferMemory, nullptr);
	
		if (hasIndexBuffer) {
			vkDestroyBuffer(ovrDevice.device(), indexBuffer, nullptr);
			vkFreeMemory(ovrDevice.device(), indexBufferMemory, nullptr);
		}
	}

	std::unique_ptr<OvrModel> OvrModel::createModelFromFile(
		OVRDevice& device, const std::string& filepath) {
		Builder builder{};
		builder.loadModel(filepath);
		std::cout << "Vertex count:" << builder.vertices.size() << "\n";
		return std::make_unique<OvrModel>(device, builder);
	}


	void OvrModel::CreateVertexBuffers(const std::vector<Vertex>& vertices)
	{
		vertexCount = static_cast<uint32_t>(vertices.size());
		assert(vertexCount >= 3 && "Vertex count must be at least 3");
		VkDeviceSize bufferSize = sizeof(vertices[0]) * vertexCount;
		
		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;

		ovrDevice.createBuffer(
			bufferSize,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			stagingBuffer,
			stagingBufferMemory
		);
		void* data;
		vkMapMemory(ovrDevice.device(), stagingBufferMemory, 0, bufferSize, 0, &data);
	
		memcpy(data, vertices.data(), static_cast<size_t>(bufferSize));
		vkUnmapMemory(ovrDevice.device(), stagingBufferMemory);
	
		ovrDevice.createBuffer(
			bufferSize,
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			vertexBuffer,
			vertexBufferMemory
		);

		ovrDevice.copyBuffer(stagingBuffer, vertexBuffer, bufferSize);

		vkDestroyBuffer(ovrDevice.device(), stagingBuffer, nullptr);
		vkFreeMemory(ovrDevice.device(), stagingBufferMemory, nullptr);
	}

	void OvrModel::CreateIndexBuffers(const std::vector<uint32_t>& indices)
	{
		indexCount = static_cast<uint32_t>(indices.size());
		hasIndexBuffer = indexCount > 0;

		if (!hasIndexBuffer) {
			return;
		}

		VkDeviceSize bufferSize = sizeof(indices[0]) * indexCount;

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;

		ovrDevice.createBuffer(
			bufferSize,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			stagingBuffer,
			stagingBufferMemory
		);
		void* data;
		vkMapMemory(ovrDevice.device(), stagingBufferMemory, 0, bufferSize, 0, &data);

		memcpy(data, indices.data(), static_cast<size_t>(bufferSize));
		vkUnmapMemory(ovrDevice.device(), stagingBufferMemory);

		ovrDevice.createBuffer(
			bufferSize,
			VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			indexBuffer,
			indexBufferMemory
		);

		ovrDevice.copyBuffer(stagingBuffer, indexBuffer, bufferSize);

		vkDestroyBuffer(ovrDevice.device(), stagingBuffer, nullptr);
		vkFreeMemory(ovrDevice.device(), stagingBufferMemory, nullptr);
	}

	void OvrModel::draw(VkCommandBuffer commandBuffer)
	{
		if (hasIndexBuffer) {
			vkCmdDrawIndexed(commandBuffer, indexCount, 1, 0, 0, 0);
		}
		else {
			vkCmdDraw(commandBuffer, vertexCount, 1, 0, 0);
		}
	}

	void OvrModel::bind(VkCommandBuffer commandBuffer)
	{
		VkBuffer buffers[] = { vertexBuffer };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);

		if (hasIndexBuffer) {
			// max vectex count for INDEX_UINT32 about 4 billion, INDEX_UINT16 - 64 000, INDEX_UINT8 - 256
			vkCmdBindIndexBuffer(commandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT32); 
		}
	}
	std::vector<VkVertexInputBindingDescription> OvrModel::Vertex::getBindingDescriptions()
	{
		std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
		bindingDescriptions[0].binding = 0;
		bindingDescriptions[0].stride = sizeof(Vertex);
		bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		return bindingDescriptions;

	}
	std::vector<VkVertexInputAttributeDescription> OvrModel::Vertex::getAttributeDescriptions()
	{
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions(2);

		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(Vertex, position);

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Vertex, color);
		return attributeDescriptions;
	}

	void OvrModel::Builder::loadModel(const std::string& filepath) {
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string warn, err;
		
		if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, filepath.c_str())) {
			//std::cout << err << std::endl;
			throw std::runtime_error(warn + err);
		}

		vertices.clear();
		indices.clear();

		std::unordered_map<Vertex, uint32_t> uniqueVertices{};

		for (const auto& shape : shapes) {
			for (const auto& index : shape.mesh.indices) {
				Vertex vertex{};

				if (index.vertex_index >= 0) {
					vertex.position = {
						attrib.vertices[3 * index.vertex_index + 0],
						attrib.vertices[3 * index.vertex_index + 1],
						attrib.vertices[3 * index.vertex_index + 2],

					};
					//std::vector<float> vertices (3.1, 2.3f, 2.3f);
					//std::vector<float> vertices(3.1, 2.3f, 2.3f);
					//std::vector<float> vertices(3.1, 2.3f, 2.3f);

					vertex.color = {
						/*vertices,
						vertices,
						vertices,*/
						//attrib.colors[3 * index.vertex_index + 0],
						//attrib.colors[3 * index.vertex_index + 1],
						//attrib.colors[3 * index.vertex_index + 2],

					};
				}

				if (index.normal_index >= 0) {
					vertex.normal = {
						attrib.normals[3 * index.normal_index + 0],
						attrib.normals[3 * index.normal_index + 1],
						attrib.normals[3 * index.normal_index + 2],

					};
				}

				if (index.texcoord_index >= 0) {
					vertex.uv = {
						attrib.texcoords[3 * index.texcoord_index + 0],
						attrib.texcoords[3 * index.texcoord_index + 1],

					};
				}
				
				if (uniqueVertices.count(vertex) == 0) {
					uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
					vertices.push_back(vertex);
				}
				indices.push_back(uniqueVertices[vertex]);
			}
		}
	}

}