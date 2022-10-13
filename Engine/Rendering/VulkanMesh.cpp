#include "CoreInclude.hpp"
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
	attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
	attributeDescriptions[0].offset = offsetof(Vertex, position);

	attributeDescriptions[1].binding = 0;
	attributeDescriptions[1].location = 1;
	attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributeDescriptions[1].offset = offsetof(Vertex, color);
	return attributeDescriptions;
}


Mesh::Mesh(Device& device, const std::vector<Vertex>& vertices) : m_Device(device) {

	_CreateVertexBuffers(vertices);
}

Mesh::~Mesh() {
	vkDestroyBuffer(m_Device.device(), m_VertexBuffer, nullptr);
	vkFreeMemory(m_Device.device(), m_VertexBufferMemory, nullptr);
}

void Mesh::Bind(VkCommandBuffer commandBuffer) {
	VkBuffer buffers[] = { m_VertexBuffer };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
}

void Mesh::Draw(VkCommandBuffer commandBuffer) {
	vkCmdDraw(commandBuffer, m_VertexCount, 1, 0, 0);
}


void Mesh::_CreateVertexBuffers(const std::vector<Vertex>& vertices) {
	m_VertexCount = Cast<uint32_t>(vertices.size());
	VkDeviceSize bufferSize = sizeof(Vertex) * m_VertexCount;
	m_Device.createBuffer(bufferSize,
						  VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
						  VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
						  m_VertexBuffer,
						  m_VertexBufferMemory);

	void* data;
	vkMapMemory(m_Device.device(), m_VertexBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, vertices.data(), Cast<size_t>(bufferSize));
	vkUnmapMemory(m_Device.device(), m_VertexBufferMemory);
}