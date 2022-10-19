#pragma once

#include "Main/CoreInclude.hpp"
#include "VulkanDevice.hpp"
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace Pit::Rendering {
	struct Vertex {
		glm::vec3 position;
		glm::vec3 color;

		static std::vector<VkVertexInputBindingDescription> GetBindingDescription();
		static std::vector<VkVertexInputAttributeDescription> GetAttributeDescription();
	};

	class Mesh {
	public:
		Mesh(Device& device, const std::vector<Vertex>& vertices);
		~Mesh();

		void Bind(VkCommandBuffer commandBuffer);
		void Draw(VkCommandBuffer commandBuffer);

		Mesh(const Mesh&) = delete;
		Mesh& operator=(const Mesh&) = delete;

	private:
		void _CreateVertexBuffers(const std::vector<Vertex>& vertices);

		Device& m_Device;
		VkBuffer m_VertexBuffer;
		VkDeviceMemory m_VertexBufferMemory;
		uint32_t m_VertexCount;
	};
}