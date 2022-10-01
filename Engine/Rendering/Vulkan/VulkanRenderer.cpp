#include "VulkanRenderer.hpp"

using namespace Pit::Rendering::Vulkan;

Renderer::Renderer() {
	_InitGLFW();
	

	vkDeviceWaitIdle(m_Device.device());

	_CreatePipelineLayout();
	_CreatePipeline();
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
	glfwSetWindowPosCallback(m_Window.GetWindowHandle(), _SetWindowPosCallback);
	glfwSetWindowSizeCallback(m_Window.GetWindowHandle(), _SetWindowSizeCallback);
	glfwSetFramebufferSizeCallback(m_Window.GetWindowHandle(), _SetFramebufferSizeCallback);

	if (ImGuiRenderer)
		delete ImGuiRenderer;
	ImGuiRenderer = new Pit::Rendering::ImGuiRenderer(m_Window.GetWindowHandle());
}

void Renderer::_CreatePipelineLayout() {
	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 0;
	pipelineLayoutInfo.pSetLayouts = nullptr;
	pipelineLayoutInfo.pushConstantRangeCount = 0;
	pipelineLayoutInfo.pPushConstantRanges = nullptr;
	if (vkCreatePipelineLayout(m_Device.device(), &pipelineLayoutInfo, nullptr, &m_PipelineLayout) != VK_SUCCESS)
		PIT_ENGINE_ERR("Failed to create Pipeline-Layout");
}

void Renderer::_CreatePipeline() {
	PipelineConfigInfo pipelineConfig{};
	Pipeline::DefaultConfigInfo(pipelineConfig, m_SwapChain.width(), m_SwapChain.height());
	pipelineConfig.renderPass = m_SwapChain.getRenderPass();
	pipelineConfig.pipelineLayout = m_PipelineLayout;
	m_Pipeline = std::make_unique<Pipeline>(m_Device,
											FileSystem::GetSandboxDir() + "/shaders/simple_shader.vert.spv",
											FileSystem::GetSandboxDir() + "/shaders/simple_shader.frag.spv",
											pipelineConfig);
}

void Renderer::_CreateCommandBuffers() {
	m_CommandBuffers.resize(m_SwapChain.imageCount());
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = m_Device.getCommandPool();
	allocInfo.commandBufferCount = static_cast<uint32_t>(m_CommandBuffers.size());

	if (vkAllocateCommandBuffers(m_Device.device(), &allocInfo, m_CommandBuffers.data()) != VK_SUCCESS)
		PIT_ENGINE_ERR("Failed to allocate CommandBuffers");

	for (size_t i = 0; i < m_CommandBuffers.size(); i++) {
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	
		if (vkBeginCommandBuffer(m_CommandBuffers[i], &beginInfo) != VK_SUCCESS)
			PIT_ENGINE_ERR("Failed to begin CommandBuffer Nr.{}", i);

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = m_SwapChain.getRenderPass();
		renderPassInfo.framebuffer = m_SwapChain.getFrameBuffer(static_cast<int>(i));

		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = m_SwapChain.getSwapChainExtent();

		VkClearValue clearValues[2];
		clearValues[0].color = { 0.1f, 0.1f, 0.1f, 1.f };
		clearValues[1].depthStencil = { 1.f, 0 };
		renderPassInfo.clearValueCount = 2;
		renderPassInfo.pClearValues = clearValues;

		vkCmdBeginRenderPass(m_CommandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		m_Pipeline->Bind(m_CommandBuffers[i]);
		vkCmdDraw(m_CommandBuffers[i], 3, 1, 0, 0);

		vkCmdEndRenderPass(m_CommandBuffers[i]);
		if (vkEndCommandBuffer(m_CommandBuffers[i]) != VK_SUCCESS)
			PIT_ENGINE_ERR("Failed to end CommandBuffer Nr.{}", i);
	}
}
void Renderer::_DrawFrame() {
	uint32_t imageIndex;
	auto result = m_SwapChain.acquireNextImage(&imageIndex);
	if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
		PIT_ENGINE_ERR("Failed to acquire next swapchain image");

	result = m_SwapChain.submitCommandBuffers(&m_CommandBuffers[imageIndex], &imageIndex);
	if (result != VK_SUCCESS)
		PIT_ENGINE_ERR("Failed to present CommandBuffer-image Nr.{}", imageIndex);
}


// Helper funcs
void Renderer::_InitGLFW() {
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
}

void Renderer::_ShutdownGLFW() {
	glfwTerminate();
}


#pragma region GLFW callbacks
void Renderer::_SetFramebufferSizeCallback(GLFWwindow* window, int width, int height) {
	if (Pit::Rendering::Renderer::Instance != nullptr)
		Pit::Rendering::Renderer::Instance->Render();
	glfwSwapBuffers(window);
}

void Renderer::_SetWindowSizeCallback(GLFWwindow* window, int width, int height) {
	if (Pit::Rendering::Renderer::Instance != nullptr)
		Pit::Rendering::Renderer::Instance->Render();
	glfwSwapBuffers(window);
}
void Renderer::_SetWindowPosCallback(GLFWwindow* window, int xpos, int ypos) {
	if (Pit::Rendering::Renderer::Instance != nullptr)
		Pit::Rendering::Renderer::Instance->Render();
	glfwSwapBuffers(window);
}
#pragma endregion