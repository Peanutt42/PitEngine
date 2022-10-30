#pragma once

#include "Core/CoreInclude.hpp"
#include "RenderingDevice.hpp"
#include "RenderingBuffer.hpp"
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

		static Array<VkVertexInputBindingDescription> GetBindingDescription();
		static Array<VkVertexInputAttributeDescription> GetAttributeDescription();

		bool operator==(const Vertex& other) const {
			return position == other.position && color == other.color && normal == other.normal &&
				uv == other.uv;
		}

		struct Builder {
			Array<Vertex> vertices;
			Array<uint32> indices;

			static Builder FromFile(const String& filepath);
		};
	};
}

// from: https://stackoverflow.com/a/57595105
template <typename T, typename... Rest>
void hashCombine(Pit::size& seed, const T& v, const Rest&... rest) {
	seed ^= std::hash<T>{}(v)+0x9e3779b9 + (seed << 6) + (seed >> 2);
	(hashCombine(seed, rest), ...);
};

namespace std {
	template <>
	struct hash<Pit::Rendering::Vertex> {
		Pit::size operator()(const Pit::Rendering::Vertex& vertex) const {
			Pit::size seed = 0;
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

		static std::unique_ptr<Mesh> CreateMeshFromFile(Device& device, const String& path);

		void Bind(VkCommandBuffer commandBuffer);
		void Draw(VkCommandBuffer commandBuffer);

		Mesh(const Mesh&) = delete;
		Mesh& operator=(const Mesh&) = delete;

	private:
		void _CreateVertexBuffers(const Array<Vertex>& vertices);
		void _CreateIndexBuffers(const Array<uint32>& indices);

		Device& m_Device;

		std::unique_ptr<Buffer> m_VertexBuffer;
		uint32 m_VertexCount;

		bool m_HasIndexBuffer = false;
		std::unique_ptr<Buffer> m_IndexBuffer;
		uint32 m_IndexCount;
	};
}