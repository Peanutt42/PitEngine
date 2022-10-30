#include "pch.hpp"
#include "Mesh.hpp"
#include <tinyobjloader/tiny_obj_loader.h>

using namespace Pit;
using namespace Rendering;


Array<VkVertexInputBindingDescription> Vertex::GetBindingDescription() {
	Array<VkVertexInputBindingDescription> bindingDescriptions(1);
	bindingDescriptions[0].binding = 0;
	bindingDescriptions[0].stride = sizeof(Vertex);
	bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	return bindingDescriptions;
}

Array<VkVertexInputAttributeDescription> Vertex::GetAttributeDescription() {
	Array<VkVertexInputAttributeDescription> attributeDescriptions {
		{ 0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, position) },
		{ 1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color) },
		{ 2, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, normal) },
		{ 3, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, uv) }
	};
	
	return attributeDescriptions;
}

Vertex::Builder Vertex::Builder::FromFile(const String& filepath) {
	Vertex::Builder builder;
	tinyobj::attrib_t attrib;
	Array<tinyobj::shape_t> shapes;
	Array<tinyobj::material_t> materials;
	String warn, err;

	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filepath.c_str()))
		PIT_ENGINE_ERR(Log::Rendering, "WARN/ERR while loading model: warning: {0}, err: {1}", warn, err);

	builder.vertices.clear();
	builder.indices.clear();

	std::unordered_map<Vertex, uint32> uniqueVertices{};
	for (const auto& shape : shapes) {
		for (const auto& index : shape.mesh.indices) {
			Vertex vertex{};

			if (index.vertex_index >= 0) {
				vertex.position = {
					attrib.vertices[3 * index.vertex_index + 0],
					attrib.vertices[3 * index.vertex_index + 1],
					attrib.vertices[3 * index.vertex_index + 2],
				};
				vertex.color= {
					attrib.colors[3 * index.vertex_index + 0],
					attrib.colors[3 * index.vertex_index + 1],
					attrib.colors[3 * index.vertex_index + 2],
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
					attrib.texcoords[2 * index.texcoord_index + 0],
					attrib.texcoords[2 * index.texcoord_index + 1],
				};
			}

			if (uniqueVertices.count(vertex) == 0) {
				uniqueVertices[vertex] = static_cast<uint32>(builder.vertices.size());
				builder.vertices.push_back(vertex);
			}
			builder.indices.push_back(uniqueVertices[vertex]);
		}
	}
	return builder;
}


Mesh::Mesh(Device& device, const Vertex::Builder& builder) : m_Device(device) {
	_CreateVertexBuffers(builder.vertices);
	_CreateIndexBuffers(builder.indices);
}

Mesh::~Mesh() {
	
}

std::unique_ptr<Mesh> Mesh::CreateMeshFromFile(Device& device, const String& path) {
	Vertex::Builder builder = Vertex::Builder::FromFile(path);
	return std::make_unique<Mesh>(device, builder);
}

void Mesh::Bind(VkCommandBuffer commandBuffer) {
	VkBuffer buffers[] = { m_VertexBuffer->getBuffer() };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);

	if (m_HasIndexBuffer)
		vkCmdBindIndexBuffer(commandBuffer, m_IndexBuffer->getBuffer(), 0, VK_INDEX_TYPE_UINT32);
}

void Mesh::Draw(VkCommandBuffer commandBuffer) {
	if (m_HasIndexBuffer)
		vkCmdDrawIndexed(commandBuffer, m_IndexCount, 1, 0, 0, 0);
	else
		vkCmdDraw(commandBuffer, m_VertexCount, 1, 0, 0);
}


void Mesh::_CreateVertexBuffers(const Array<Vertex>& vertices) {
	m_VertexCount = Cast<uint32>(vertices.size());
	VkDeviceSize bufferSize = sizeof(Vertex) * m_VertexCount;
	uint32 vertexSize = sizeof(Vertex);

	Buffer stagingBuffer(m_Device,
						 vertexSize,
						 m_VertexCount,
						 VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
						 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	stagingBuffer.map();
	stagingBuffer.writeToBuffer((void*)vertices.data());

	m_VertexBuffer = std::make_unique<Buffer>(m_Device,
							vertexSize,
							m_VertexCount,
							VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
							VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	m_Device.copyBuffer(stagingBuffer.getBuffer(), m_VertexBuffer->getBuffer(), bufferSize);
}

void Mesh::_CreateIndexBuffers(const Array<uint32>& indices) {
	m_IndexCount = Cast<uint32>(indices.size());
	m_HasIndexBuffer = m_IndexCount > 0;
	if (!m_HasIndexBuffer) return;

	VkDeviceSize bufferSize = sizeof(uint32) * m_IndexCount;
	uint32 indexSize = sizeof(uint32);

	Buffer stagingBuffer(m_Device,
						 indexSize,
						 m_IndexCount,
						 VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
						 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	stagingBuffer.map();
	stagingBuffer.writeToBuffer((void*)indices.data());

	m_IndexBuffer = std::make_unique<Buffer>(m_Device,
											 indexSize,
											 m_IndexCount,
											 VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
											 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	m_Device.copyBuffer(stagingBuffer.getBuffer(), m_IndexBuffer->getBuffer(), bufferSize);
}