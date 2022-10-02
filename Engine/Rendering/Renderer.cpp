#include "Renderer.hpp"

#include <filesystem>
namespace fs = std::filesystem;

using namespace Pit::Rendering;



Renderer::Renderer(Device& device, Window& window, ECS::World* ecsWorld) : m_CurrentWindow(window), m_Device(device), m_ECSWorld(ecsWorld) {
	_InitGLFW();

	vkDeviceWaitIdle(m_Device.device());
	_RecreateSwapChain();
	_CreateCommandBuffers();
	
	// TODO: improve
	if (m_ImGuiRenderer)
		delete m_ImGuiRenderer;
	m_ImGuiRenderer = new Pit::Rendering::ImGuiRenderer(m_CurrentWindow.GetWindowHandle());

	m_CurrentWindow.SetRenderer(this);
}

Renderer::~Renderer() {
	vkDeviceWaitIdle(m_Device.device());

	_FreeCommandBuffers();
	_ShutdownGLFW();

	delete m_ImGuiRenderer;
}

void Renderer::SetSpecs(const RendererSpecs& specs) {
	PIT_ENGINE_INFO("TODO: Imploment Vsync, vsync: {}", specs.VSync);
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
}
void Renderer::BeginSwapChainRenderPass(VkCommandBuffer commandBuffer) {
	assert(m_IsFrameStarted && "Can't call BeginSwapChainRenderPass() while frame isn't in progress");
	assert(commandBuffer == GetCurrentCommandBuffer() &&
		   "Can't begin renderPass on commandBuffer from diffrent frame!");
	
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
	assert(commandBuffer == GetCurrentCommandBuffer() &&
		   "Can't end renderPass on commandBuffer from diffrent frame!");

	vkCmdEndRenderPass(commandBuffer);
}
 
bool Renderer::ShouldClose() {
	return m_CurrentWindow.ShouldClose();
}


void Renderer::_CreateCommandBuffers() {
	m_CommandBuffers.resize(m_SwapChain->imageCount());

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
		m_SwapChain = std::make_unique<SwapChain>(m_Device, extent, std::move(m_SwapChain));
		if (m_SwapChain->imageCount() != m_CommandBuffers.size()) {
			_FreeCommandBuffers();
			_CreateCommandBuffers();
		}
	}
}

void Renderer::_InitGLFW() {
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
}
void Renderer::_ShutdownGLFW() {
	glfwTerminate();
}