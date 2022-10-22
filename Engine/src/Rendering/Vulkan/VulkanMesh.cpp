#include "Main/CoreInclude.hpp"
#include "VulkanMesh.hpp"

using namespace Pit::Rendering;


std::vector<VkVertexInputBindingDescription> Vertex::GetBindingDescription() {
	std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
	bindingDescriptions[0].binding = 0;
	bindingDescriptions[0].stride = sizeof(Vertex);
	bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	return bindingDescriptions;
}

std::vector<VkVertexInputAttributeDescription> Vertex::GetAttributeDescription() {
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
