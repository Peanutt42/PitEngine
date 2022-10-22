#include "Main/CoreInclude.hpp"
#include "VulkanMesh.hpp"
#include <tinyobjloader/tiny_obj_loader.h>

using namespace Pit::Rendering;


std::vector<VkVertexInputBindingDescription> Vertex::GetBindingDescription() {
	std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
	bindingDescriptions[0].binding = 0;
	bindingDescriptions[0].stride = sizeof(Vertex);
	bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	return bindingDescriptions;
}

std::vector<VkVertexInputAttributeDescription> Vertex::GetAttributeDescription() {
	std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};
	attributeDescriptions.push_back({ 0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, position) });
	attributeDescriptions.push_back({ 1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color) });
	attributeDescriptions.push_back({ 2, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, normal) });
	attributeDescriptions.push_back({ 3, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, uv) });
	
	return attributeDescriptions;
}

Vertex::Builder Vertex::Builder::FromFile(const std::string& filepath) {
	Vertex::Builder builder;
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warn, err;

	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filepath.c_str()))
		PIT_ENGINE_ERR(Log::Rendering, "WARN/ERR while loading model: warning: {0}, err: {1}", warn, err);

	builder.vertices.clear();
	builder.indices.clear();

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
				uniqueVertices[vertex] = static_cast<uint32_t>(builder.vertices.size());
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
	vkDestroyBuffer(m_Device.device(), m_VertexBuffer, nullptr);
	vkFreeMemory(m_Device.device(), m_VertexBufferMemory, nullptr);
	if (m_HasIndexBuffer) {
		vkDestroyBuffer(m_Device.device(), m_IndexBuffer, nullptr);
		vkFreeMemory(m_Device.device(), m_IndexBufferMemory, nullptr);
	}
}

std::unique_ptr<Mesh> Mesh::CreateMeshFromFile(Device& device, const std::string& path) {
	Vertex::Builder builder = Vertex::Builder::FromFile(path);
	return std::make_unique<Mesh>(device, builder);
}

void Mesh::Bind(VkCommandBuffer commandBuffer) {
	VkBuffer buffers[] = { m_VertexBuffer };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);

	if (m_HasIndexBuffer)
		vkCmdBindIndexBuffer(commandBuffer, m_IndexBuffer, 0, VK_INDEX_TYPE_UINT32);
}

void Mesh::Draw(VkCommandBuffer commandBuffer) {
	if (m_HasIndexBuffer)
		vkCmdDrawIndexed(commandBuffer, m_IndexCount, 1, 0, 0, 0);
	else
		vkCmdDraw(commandBuffer, m_VertexCount, 1, 0, 0);
}


void Mesh::_CreateVertexBuffers(const std::vector<Vertex>& vertices) {
	m_VertexCount = Cast<uint32_t>(vertices.size());
	VkDeviceSize bufferSize = sizeof(Vertex) * m_VertexCount;

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;


	m_Device.createBuffer(bufferSize,
						  VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
						  VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
						  stagingBuffer,
						  stagingBufferMemory);

	void* data;
	vkMapMemory(m_Device.device(), stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, vertices.data(), Cast<size_t>(bufferSize));
	vkUnmapMemory(m_Device.device(), stagingBufferMemory);

	m_Device.createBuffer(bufferSize,
						  VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
						  VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
						  m_VertexBuffer,
						  m_VertexBufferMemory);

	m_Device.copyBuffer(stagingBuffer, m_VertexBuffer, bufferSize);

	vkDestroyBuffer(m_Device.device(), stagingBuffer, nullptr);
	vkFreeMemory(m_Device.device(), stagingBufferMemory, nullptr);
}

void Mesh::_CreateIndexBuffers(const std::vector<uint32_t>& indices) {
	m_IndexCount = Cast<uint32_t>(indices.size());
	m_HasIndexBuffer = m_IndexCount > 0;
	if (!m_HasIndexBuffer) return;

	VkDeviceSize bufferSize = sizeof(uint32_t) * m_IndexCount;
	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;


	m_Device.createBuffer(bufferSize,
						  VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
						  VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
						  stagingBuffer,
						  stagingBufferMemory);

	void* data;
	vkMapMemory(m_Device.device(), stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, indices.data(), Cast<size_t>(bufferSize));
	vkUnmapMemory(m_Device.device(), stagingBufferMemory);

	m_Device.createBuffer(bufferSize,
						  VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
						  VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
						  m_IndexBuffer,
						  m_IndexBufferMemory);

	m_Device.copyBuffer(stagingBuffer, m_IndexBuffer, bufferSize);

	vkDestroyBuffer(m_Device.device(), stagingBuffer, nullptr);
	vkFreeMemory(m_Device.device(), stagingBufferMemory, nullptr);
}