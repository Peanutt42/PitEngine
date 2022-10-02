#include "VulkanRenderer.hpp"

using namespace Pit::Rendering::Vulkan;

struct SimplePushConstantData {
	glm::mat2 transform{1.0f};
	glm::vec2 offset;
	alignas(16) glm::vec3 color;
};

Renderer::Renderer() {
	_InitGLFW();
	
	vkDeviceWaitIdle(m_Device.device());
	_LoadModels();
	_CreatePipelineLayout();
	_RecreateSwapChain();
	_CreateCommandBuffers();
}

Renderer::~Renderer() {
	vkDeviceWaitIdle(m_Device.device());

	_ShutdownGLFW();

	vkDestroyPipelineLayout(m_Device.device(), m_PipelineLayout, nullptr);

	delete ImGuiRenderer;
}

void Renderer::Render() {
	glfwPollEvents();

	ImGuiRenderer->NewFrame();
	ImGuiRenderer->_TestWindow();
	ImGuiRenderer->Render();

	m_Window.Update();

	_DrawFrame();
}

void Renderer::SetSpecs(const RendererSpecs& specs) {
	PIT_ENGINE_INFO("TODO: Imploment Vsync, vsync: {}", specs.VSync);
}

bool Renderer::ShouldClose() {
	return m_Window.ShouldClose();
}

void Renderer::SetWindow(Window* window) {
	// TODO: improve
	if (ImGuiRenderer)
		delete ImGuiRenderer;
	ImGuiRenderer = new Pit::Rendering::ImGuiRenderer(m_Window.GetWindowHandle());
}

void Renderer::_CreatePipelineLayout() {
	VkPushConstantRange pushConstantRange{};
	pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
	pushConstantRange.offset = 0;
	pushConstantRange.size = sizeof(SimplePushConstantData);

	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 0;
	pipelineLayoutInfo.pSetLayouts = nullptr;
	pipelineLayoutInfo.pushConstantRangeCount = 1;
	pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
	if (vkCreatePipelineLayout(m_Device.device(), &pipelineLayoutInfo, nullptr, &m_PipelineLayout) != VK_SUCCESS)
		PIT_ENGINE_ERR("Failed to create Pipeline-Layout");
}

void Renderer::_CreatePipeline() {
	assert(m_SwapChain != nullptr && "Cannot create pipeline before swap chain");
	assert(m_PipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

	PipelineConfigInfo pipelineConfig{};
	Pipeline::DefaultConfigInfo(pipelineConfig);
	pipelineConfig.renderPass = m_SwapChain->getRenderPass();
	pipelineConfig.pipelineLayout = m_PipelineLayout;
	m_Pipeline = std::make_unique<Pipeline>(m_Device,
											FileSystem::GetSandboxDir() + "/shaders/simple_shader.vert.spv",
											FileSystem::GetSandboxDir() + "/shaders/simple_shader.frag.spv",
											pipelineConfig);
}

void Renderer::_CreateCommandBuffers() {
	m_CommandBuffers.resize(m_SwapChain->imageCount());

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = m_Device.getCommandPool();
	allocInfo.commandBufferCount = static_cast<uint32_t>(m_CommandBuffers.size());

	if (vkAllocateCommandBuffers(m_Device.device(), &allocInfo, m_CommandBuffers.data()) != VK_SUCCESS)
		PIT_ENGINE_ERR("Failed to allocate CommandBuffers");
}
void Renderer::_DrawFrame() {
	uint32_t imageIndex;
	auto result = m_SwapChain->acquireNextImage(&imageIndex);
	if (result == VK_ERROR_OUT_OF_DATE_KHR) {
		_RecreateSwapChain();
		return;
	}

	if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
		PIT_ENGINE_ERR("Failed to acquire next swapchain image");

	_RecordCommandBuffer(imageIndex);
	result = m_SwapChain->submitCommandBuffers(&m_CommandBuffers[imageIndex], &imageIndex);
	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_Window.WasResized()) {
		m_Window.ResetResizedFlag();
		_RecreateSwapChain();
		return;
	}
	else if (result != VK_SUCCESS)
		PIT_ENGINE_ERR("Failed to present CommandBuffer-image Nr.{}", imageIndex);
}

void Renderer::_Sierpinski(std::vector<Vertex>& vertices, int depth, glm::vec2 left, glm::vec2 right, glm::vec2 top) {
	if (depth <= 0) {
		vertices.push_back({ top });
		vertices.push_back({ right });
		vertices.push_back({ left });
	}
	else {
		auto leftTop = 0.5f * (left + top);
		auto rightTop = 0.5f * (right + top);
		auto leftRight = 0.5f * (left + right);
		_Sierpinski(vertices, depth - 1, left, leftRight, leftTop);
		_Sierpinski(vertices, depth - 1, leftRight, right, rightTop);
		_Sierpinski(vertices, depth - 1, leftTop, rightTop, top);
	}
}

void Renderer::_LoadModels() {
#if true
	std::vector<Vertex> vertices{};
	_Sierpinski(vertices, 3, { -0.5f, 0.5f }, { 0.5f, 0.5f }, { 0.0f, -0.5f });
#else
	std::vector<Vertex> vertices{
	  {{0.0f, -0.5f}},
	  {{0.5f, 0.5f}},
	  {{-0.5f, 0.5f}} };
#endif
	m_Model = std::make_unique<Model>(m_Device, vertices);
}

void Renderer::_RecordCommandBuffer(int imageIndex) {
	static int frame = 0;
	frame = (frame + 1) % 100;

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

	if (vkBeginCommandBuffer(m_CommandBuffers[imageIndex], &beginInfo) != VK_SUCCESS) {
		throw std::runtime_error("Failed to begin recording command buffer!");
	}

	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = m_SwapChain->getRenderPass();
	renderPassInfo.framebuffer = m_SwapChain->getFrameBuffer(imageIndex);

	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = m_SwapChain->getSwapChainExtent();

	std::array<VkClearValue, 2> clearValues{};
	clearValues[0].color = { 0.01f, 0.01f, 0.01f, 1.0f };
	clearValues[1].depthStencil = { 1.0f, 0 };
	renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
	renderPassInfo.pClearValues = clearValues.data();

	vkCmdBeginRenderPass(m_CommandBuffers[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

	VkViewport viewport{};
	viewport.x = viewport.y = 0.0f;
	viewport.width = static_cast<float>(m_SwapChain->getSwapChainExtent().width);
	viewport.height = static_cast<float>(m_SwapChain->getSwapChainExtent().height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	VkRect2D scissor{ {0, 0}, m_SwapChain->getSwapChainExtent() };
	vkCmdSetViewport(m_CommandBuffers[imageIndex], 0, 1, &viewport);
	vkCmdSetScissor(m_CommandBuffers[imageIndex], 0, 1, &scissor);

	m_Pipeline->Bind(m_CommandBuffers[imageIndex]);
	m_Model->Bind(m_CommandBuffers[imageIndex]);

	for (int j = 0; j < 4; j++) {
		SimplePushConstantData push{};
		push.offset = { -0.5f + frame * 0.02f, -0.4f + j * 0.25f };
		push.color = { 0.0f, 0.0f, 0.2f + 0.2f * j };

		vkCmdPushConstants(m_CommandBuffers[imageIndex], m_PipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(SimplePushConstantData), &push);
		m_Model->Draw(m_CommandBuffers[imageIndex]);
	}

	vkCmdEndRenderPass(m_CommandBuffers[imageIndex]);
	if (vkEndCommandBuffer(m_CommandBuffers[imageIndex]) != VK_SUCCESS)
		PIT_ENGINE_ERR("Failed to end CommandBuffer Nr.{}", imageIndex);
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
	auto extent = m_Window.GetExtent();
	while (extent.width == 0 || extent.height == 0) {
		extent = m_Window.GetExtent();
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

	_CreatePipeline();
}

// GLFW Helper funcs
void Renderer::_InitGLFW() {
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
}

void Renderer::_ShutdownGLFW() {
	glfwTerminate();
}