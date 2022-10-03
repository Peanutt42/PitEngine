#pragma once

#include "Rendering/RenderingInclude.hpp"
#include "VulkanDevice.hpp"

namespace Pit::Rendering::Vulkan {
	struct Vertex {
		glm::vec3 position;
		glm::vec3 color;

		
		static std::vector<VkVertexInputBindingDescription> GetBindingDescriptions();
		static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions();
	};

	class Model {
	public:
		Model(Device& device, const std::vector<Vertex>& vertices);
		~Model();

		Model(const Model&) = delete;
		Model& operator=(const Model&) = delete;

		void Bind(VkCommandBuffer commandBuffer);
		void Draw(VkCommandBuffer commandBuffer);

	private:
		void _CreateVertexBuffers(const std::vector<Vertex>& vertices);

		Device& m_Device;
		VkBuffer m_VertexBuffer;
		VkDeviceMemory m_VertexBufferMemory;
		uint32_t m_VertexCount;
	};
}