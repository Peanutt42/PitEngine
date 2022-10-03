#include "Renderer.hpp"

#include <filesystem>
namespace fs = std::filesystem;

using namespace Pit::Rendering;



Renderer::Renderer(Device& device, Window& window, ECS::World* ecsWorld) : m_CurrentWindow(window), m_Device(device), m_ECSWorld(ecsWorld) {
	_InitGLFW();

	vkDeviceWaitIdle(m_Device.device());
	_RecreateSwapChain();
	_CreateCommandBuffers();

	m_CurrentWindow.SetRenderer(this);
}

Renderer::~Renderer() {
	vkDeviceWaitIdle(m_Device.device());

	vkDestroyDescriptorPool(m_Device.device(), m_DescriptorPool, nullptr);
	_FreeCommandBuffers();
	_ShutdownGLFW();
}

VkCommandBuffer Renderer::BeginFrame() {
	assert(!m_IsFrameStarted && "Can't call BeginFrame() while already in progress!");

	auto result = m_SwapChain->acquireNextImage(&m_CurrentImageIndex);
	if (result == VK_ERROR_OUT_OF_DATE_KHR) {
		_RecreateSwapChain();
		return nullptr;
	}
	if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
		PIT_ENGINE_ERR("Failed to acquire SwapChain-Image!");

	m_IsFrameStarted = true;

	auto commandBuffer = GetCurrentCommandBuffer();

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

	if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
		PIT_ENGINE_ERR("Failed to begin recording commandBuffer!");

	return commandBuffer;
}

void Renderer::BeginSwapChainRenderPass(VkCommandBuffer commandBuffer) {
	assert(m_IsFrameStarted && "Can't call BeginSwapChainRenderPass() while frame isn't in progress");
	assert(commandBuffer == GetCurrentCommandBuffer() && "Can't begin renderPass on commandBuffer from diffrent frame!");
	
	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = m_SwapChain->getRenderPass();
	renderPassInfo.framebuffer = m_SwapChain->getFrameBuffer(m_CurrentImageIndex);

	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = m_SwapChain->getSwapChainExtent();

	std::array<VkClearValue, 2> clearValues{};
	clearValues[0].color = { 0.01f, 0.01f, 0.01f, 1.0f };
	clearValues[1].depthStencil = { 1.0f, 0 };
	renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
	renderPassInfo.pClearValues = clearValues.data();

	vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

	VkViewport viewport{};
	viewport.x = viewport.y = 0.0f;
	viewport.width = static_cast<float>(m_SwapChain->getSwapChainExtent().width);
	viewport.height = static_cast<float>(m_SwapChain->getSwapChainExtent().height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	VkRect2D scissor{ {0, 0}, m_SwapChain->getSwapChainExtent() };
	vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
	vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
}

void Renderer::EndSwapChainRenderPass(VkCommandBuffer commandBuffer) {
	assert(m_IsFrameStarted && "Can't call EndSwapChainRenderPass() while frame isn't in progress");
	assert(commandBuffer == GetCurrentCommandBuffer() && "Can't end renderPass on commandBuffer from diffrent frame!");

	vkCmdEndRenderPass(commandBuffer);
}

void Renderer::EndFrame() {
	assert(m_IsFrameStarted && "Can't call EndFrame() while frame isn't in progress");

	auto commandBuffer = GetCurrentCommandBuffer();

	if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
		PIT_ENGINE_ERR("Failed to record commandBuffer!");

	auto result = m_SwapChain->submitCommandBuffers(&commandBuffer, &m_CurrentImageIndex);
	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_CurrentWindow.WasResized()) {
		m_CurrentWindow.ResetResizedFlag();
		_RecreateSwapChain();
	}
	else if (result != VK_SUCCESS)
		PIT_ENGINE_ERR("Failed to present CommandBuffer-image Nr.{}", m_CurrentImageIndex);

	m_IsFrameStarted = false;
	m_CurrentFrameIndex = (m_CurrentFrameIndex + 1) % SwapChain::MAX_FRAMES_IN_FLIGHT;
}
 
bool Renderer::ShouldClose() {
	return m_CurrentWindow.ShouldClose();
}


void Renderer::_CreateCommandBuffers() {
	m_CommandBuffers.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = static_cast<uint32_t>(m_CommandBuffers.size());
	allocInfo.commandPool = m_Device.getCommandPool();

	if (vkAllocateCommandBuffers(m_Device.device(), &allocInfo, m_CommandBuffers.data()) != VK_SUCCESS)
		PIT_ENGINE_ERR("Failed to allocate CommandBuffers");
}
void Renderer::_FreeCommandBuffers() {
	vkFreeCommandBuffers(
		m_Device.device(),
		m_Device.getCommandPool(),
		static_cast<uint32_t>(m_CommandBuffers.size()),
		m_CommandBuffers.data());
	m_CommandBuffers.clear();
}

void Renderer::_RecreateSwapChain() {
	auto extent = m_CurrentWindow.GetExtent();
	while (extent.width == 0 || extent.height == 0) {
		extent = m_CurrentWindow.GetExtent();
		glfwWaitEvents();
	}

	vkDeviceWaitIdle(m_Device.device());

	if (m_SwapChain == nullptr)
		m_SwapChain = std::make_unique<SwapChain>(m_Device, extent);
	else {
		std::shared_ptr<SwapChain> oldSwapChain = std::move(m_SwapChain);
		m_SwapChain = std::make_unique<SwapChain>(m_Device, extent, oldSwapChain);

		if (!oldSwapChain->CompareSwapChains(*m_SwapChain.get()))
			PIT_ENGINE_WARN("SwapChain image(or depth) format has changed!");
	}
}

void Renderer::_CreateDecriptorPool() {
	VkDescriptorPoolSize pool_sizes[] =
	{
		{ VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
		{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
		{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
		{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
	};
	VkDescriptorPoolCreateInfo pool_info = {};
	pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
	pool_info.maxSets = 1000 * IM_ARRAYSIZE(pool_sizes);
	pool_info.poolSizeCount = (uint32_t)IM_ARRAYSIZE(pool_sizes);
	pool_info.pPoolSizes = pool_sizes;
	if (vkCreateDescriptorPool(m_Device.device(), &pool_info, nullptr, &m_DescriptorPool) != VK_SUCCESS)
		PIT_ENGINE_ERR("Failed to create descriptorPool!");
}

void Renderer::_InitGLFW() {
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
}
void Renderer::_ShutdownGLFW() {
	glfwTerminate();
}