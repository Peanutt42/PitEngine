#pragma once

#include "Core/CoreInclude.hpp"
#include "VulkanDevice.hpp"
#include "VulkanBuffer.hpp"
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>

namespace Pit::Rendering {
	struct Vertex {
		glm::vec3 position;
		glm::vec3 color;
		glm::vec3 normal;
		glm::vec2 uv;

		static std::vector<VkVertexInputBindingDescription> GetBindingDescription();
		static std::vector<VkVertexInputAttributeDescription> GetAttributeDescription();

		bool operator==(const Vertex& other) const {
			return position == other.position && color == other.color && normal == other.normal &&
				uv == other.uv;
		}

		struct Builder {
			std::vector<Vertex> vertices;
			std::vector<uint32_t> indices;

			static Builder FromFile(const std::string& filepath);
		};
	};
}

// from: https://stackoverflow.com/a/57595105
template <typename T, typename... Rest>
void hashCombine(std::size_t& seed, const T& v, const Rest&... rest) {
	seed ^= std::hash<T>{}(v)+0x9e3779b9 + (seed << 6) + (seed >> 2);
	(hashCombine(seed, rest), ...);
};

namespace std {
	template <>
	struct hash<Pit::Rendering::Vertex> {
		size_t operator()(const Pit::Rendering::Vertex& vertex) const {
			size_t seed = 0;
			hashCombine(seed, vertex.position, vertex.color, vertex.normal, vertex.uv);
			return seed;
		}
	};
}

namespace Pit::Rendering {
	class Mesh {
	public:
		Mesh(Device& device, const Vertex::Builder& builder);
		~Mesh();

		static std::unique_ptr<Mesh> CreateMeshFromFile(Device& device, const std::string& path);

		void Bind(VkCommandBuffer commandBuffer);
		void Draw(VkCommandBuffer commandBuffer);

		Mesh(const Mesh&) = delete;
		Mesh& operator=(const Mesh&) = delete;

	private:
		void _CreateVertexBuffers(const std::vector<Vertex>& vertices);
		void _CreateIndexBuffers(const std::vector<uint32_t>& indices);

		Device& m_Device;

		std::unique_ptr<Buffer> m_VertexBuffer;
		uint32_t m_VertexCount;

		bool m_HasIndexBuffer = false;
		std::unique_ptr<Buffer> m_IndexBuffer;
		uint32_t m_IndexCount;
	};
}