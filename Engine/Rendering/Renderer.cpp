#include "CoreInclude.hpp"
#include "Renderer.hpp"
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include "Main/Engine.hpp"

using namespace Pit::Rendering;

struct SimplePushConstantData {
	glm::vec2 offset;
	alignas(16) glm::vec3 color;
};

Renderer::Renderer() {
	_LoadModels();
	_CreatePipelineLayout();
	_RecreateSwapChain();
	_CreateCommandBuffers();
	_CreateDescriptorPool();
}

Renderer::~Renderer() {
	vkDeviceWaitIdle(m_Device.device());

	vkDestroyDescriptorPool(m_Device.device(), m_DescriptorPool, nullptr);
	vkDestroyPipelineLayout(m_Device.device(), m_PipelineLayout, nullptr);
}

bool Renderer::ShouldClose() {
	return m_Window.ShouldClose();
}

void Renderer::Update() {
	Window::UpdateAllWindows();

	_DrawFrame();
}


void Renderer::_DrawFrame() {
	if (m_Window.IsMinimized()) return;

	if (m_Window.WasWindowResized()) {
		int width, height;
		glfwGetFramebufferSize(m_Window.GetWindowHandle(), &width, &height);
		if (width <= 0 || height <= 0) return;

		_RecreateSwapChain();
		m_Window.SetWindowResizedFlag(false);
	}

	Engine::Instance->UIRenderer->BeginFrame();
	Engine::Instance->UIRenderer->DrawLayers();
	Engine::Instance->UIRenderer->EndFrame();

	const bool isMinimized = ImGui::GetDrawData()->DisplaySize.x <= 0 ||
		ImGui::GetDrawData()->DisplaySize.y <= 0;

	if (!isMinimized) {
		uint32_t imageIndex = -1;
		auto result = m_SwapChain->acquireNextImage(imageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
			m_Window.SetWindowResizedFlag(true);
			return;
		}
		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
			PIT_ENGINE_FATAL(Log::Rendering, "Failed to acquire swapChainImage!");

		_RecordCommandBuffer(imageIndex);

		result = m_SwapChain->submitCommandBuffers(&m_CommandBuffers[imageIndex], &imageIndex);

		if (result != VK_SUCCESS)
			PIT_ENGINE_FATAL(Log::Rendering, "Failed to present swapChainImage!");
	}

	// Update floating ImGui windows
	if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}
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
		PIT_ENGINE_FATAL(Log::Rendering, "Failed to create pipelineLayout!");
}

void Renderer::_CreatePipeline() {
	PipelineConfigInfo pipelineConfig{};
	Pipeline::DefaultConfigInfo(pipelineConfig);
	pipelineConfig.renderPass = m_SwapChain->getRenderPass();
	pipelineConfig.pipelineLayout = m_PipelineLayout;
	m_Pipeline = std::make_unique<Pipeline>(m_Device,
											pipelineConfig,
											FileSystem::GetSandboxDir() + "assets/shaders/vert.spv",
											FileSystem::GetSandboxDir() + "assets/shaders/frag.spv");

}

static int _GetMinImageCountFromPresentMode(VkPresentModeKHR present_mode) {
	if (present_mode == VK_PRESENT_MODE_MAILBOX_KHR)
		return 3;
	if (present_mode == VK_PRESENT_MODE_FIFO_KHR || present_mode == VK_PRESENT_MODE_FIFO_RELAXED_KHR)
		return 2;
	if (present_mode == VK_PRESENT_MODE_IMMEDIATE_KHR)
		return 1;
	PIT_ENGINE_FATAL(Log::Rendering, "presentMode is not supported!");
	return 1;
}

void Renderer::_RecreateSwapChain() {
	VkExtent2D extent{ Cast<uint32_t>(m_Window.GetWidth()), Cast<uint32_t>(m_Window.GetHeight()) };
	while (extent.width == 0 || extent.height == 0) {
		extent = { Cast<uint32_t>(m_Window.GetWidth()), Cast<uint32_t>(m_Window.GetHeight()) };
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

	m_MinImageCount = _GetMinImageCountFromPresentMode(m_SwapChain->GetPresentMode());
	//ImGui_ImplVulkan_SetMinImageCount(_GetMinImageCountFromPresentMode(m_SwapChain->GetPresentMode()));
}

void Renderer::_CreateCommandBuffers() {
	m_CommandBuffers.resize(m_SwapChain->imageCount());

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = m_Device.getCommandPool();
	allocInfo.commandBufferCount = static_cast<uint32_t>(m_CommandBuffers.size());

	if (vkAllocateCommandBuffers(m_Device.device(), &allocInfo, m_CommandBuffers.data()) != VK_SUCCESS)
		PIT_ENGINE_FATAL(Log::Rendering, "Failed to create commandBuffers!");
}

void Renderer::_FreeCommandBuffers() {
	vkFreeCommandBuffers(m_Device.device(), m_Device.getCommandPool(), Cast<uint32_t>(m_CommandBuffers.size()), m_CommandBuffers.data());
	m_CommandBuffers.clear();
}

void Renderer::_RecordCommandBuffer(int imageIndex) {
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

	if (vkBeginCommandBuffer(m_CommandBuffers[imageIndex], &beginInfo) != VK_SUCCESS)
		PIT_ENGINE_FATAL(Log::Rendering, "Failed to Begin commandBuffer!");

	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = m_SwapChain->getRenderPass();
	renderPassInfo.framebuffer = m_SwapChain->getFrameBuffer(imageIndex);
	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = m_SwapChain->getSwapChainExtent();

	VkClearValue clearValues[2];
	clearValues[0].color = { .1f, .1f, .1f, 1.f };
	clearValues[1].depthStencil = { 1.f, 0 };
	renderPassInfo.clearValueCount = 2;
	renderPassInfo.pClearValues = clearValues;

	vkCmdBeginRenderPass(m_CommandBuffers[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

	VkViewport viewport{};
	viewport.x = viewport.y = 0;
	viewport.width = Cast<float>(m_SwapChain->getSwapChainExtent().width);
	viewport.height = Cast<float>(m_SwapChain->getSwapChainExtent().height);
	viewport.minDepth = 0.f;
	viewport.maxDepth = 1.f;
	VkRect2D scissors{ {0, 0}, m_SwapChain->getSwapChainExtent() };
	vkCmdSetViewport(m_CommandBuffers[imageIndex], 0, 1, &viewport);
	vkCmdSetScissor(m_CommandBuffers[imageIndex], 0, 1, &scissors);

	m_Pipeline->Bind(m_CommandBuffers[imageIndex]);
	m_TestMesh->Bind(m_CommandBuffers[imageIndex]);

	for (int j = 0; j < 4; j++) {
		SimplePushConstantData push{};
		push.offset = { -0.5f + Time::Frame() * 0.001f, -0.4f + j * 0.25f};
		push.color = { 0.f, 0.f, 0.2f + 0.2f * j };
		vkCmdPushConstants(m_CommandBuffers[imageIndex], m_PipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(SimplePushConstantData), &push);
		m_TestMesh->Draw(m_CommandBuffers[imageIndex]);
	}

	Engine::Instance->UIRenderer->Render(m_CommandBuffers[imageIndex]);

	vkCmdEndRenderPass(m_CommandBuffers[imageIndex]);
	if (vkEndCommandBuffer(m_CommandBuffers[imageIndex]) != VK_SUCCESS)
		PIT_ENGINE_ERR(Log::Rendering, "Failed to record commandBuffer!");
}

void Renderer::_CreateDescriptorPool() {
	VkDescriptorPoolSize pool_sizes[11] =
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
	pool_info.maxSets = 1000 * 11/*pool_size.size()*/;
	pool_info.poolSizeCount = 11/*pool_size.size()*/;
	pool_info.pPoolSizes = pool_sizes;
	if (vkCreateDescriptorPool(m_Device.device(), &pool_info, nullptr, &m_DescriptorPool) != VK_SUCCESS)
		PIT_ENGINE_FATAL(Log::Rendering, "Failed to create descriptorPool!");
}




void Renderer::_Sierpinski(
	std::vector<Vertex>& vertices, int depth, glm::vec2 left, glm::vec2 right, glm::vec2 top) {
	if (depth <= 0) {
		vertices.push_back({ top, {1.f, 0.f, 0.f} });
		vertices.push_back({ right, {0.f, 1.f, 0.f} });
		vertices.push_back({ left, {0.f, 0.f, 1.f} });
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
	std::vector<Vertex> vertices = {
		{{ -0.5f,0.5f }, {1.f, 0.f, 0.f}},
		{{ 0.5f,0.5f }, {0.f, 1.f, 0.f}},
		{{ 0.f, -0.5f }, {0.f, 0.f, 1.f}}
	};
	
	m_TestMesh = std::make_unique<Mesh>(m_Device, vertices);
}