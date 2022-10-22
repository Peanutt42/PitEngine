#include "Main/CoreInclude.hpp"
#include "Main/Engine.hpp"
#include "Renderer.hpp"
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include "Main/Engine.hpp"

using namespace Pit::Rendering;

DEFINE_EXTERN_PROFILE_STAT_FLOAT(RenderingRender, Rendering);
DEFINE_EXTERN_PROFILE_STAT_FLOAT(RenderingPresent, Rendering);

Renderer::Renderer() 
	: Window(Pit::Engine::GetInfo().WindowName, 800, 600),
	  Device(Window) {
	SetGLFWWindowIcon(Window.GetWindowHandle(), FileSystem::GetEngineDir() + "assets/Icons/PitEngineLogo.png");
	_LoadModels();
	_RecreateSwapChain();
	_CreateCommandBuffers();
	_CreateDescriptorPool();
}

Renderer::~Renderer() {
	vkDeviceWaitIdle(Device.device());

	vkDestroyDescriptorPool(Device.device(), DescriptorPool, nullptr);
}

bool Renderer::ShouldClose() {
	return Window.ShouldClose();
}

void Renderer::Update() {
	Window::UpdateAllWindows();

	if (Window.IsMinimized()) return;
	_BeginFrame();

	Engine::Rendering()->UIRenderer->Record();

	_RenderFrame();
	_PresentFrame();
}

void Renderer::_BeginFrame() {
	if (Window.WasWindowResized()) {
		int width, height;
		glfwGetFramebufferSize(Window.GetWindowHandle(), &width, &height);
		if (width <= 0 || height <= 0) return;

		_RecreateSwapChain();
		Window.SetWindowResizedFlag(false);
	}
}

void Renderer::_RenderFrame() {
	SCOPE_STAT(RenderingRender);

	auto result = SwapChain->acquireNextImage(ImageIndex);

	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
		Window.SetWindowResizedFlag(true);
		return;
	}
	if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
		PIT_ENGINE_FATAL(Log::Rendering, "Failed to acquire swapChainImage!");

	_RecordCommandBuffer();
}

void Renderer::_PresentFrame() {
	SCOPE_STAT(RenderingPresent);

	VkResult result = SwapChain->submitCommandBuffers(&CommandBuffers[FrameIndex], &ImageIndex);

	if (result != VK_SUCCESS)
		PIT_ENGINE_FATAL(Log::Rendering, "Failed to present swapChainImage!");

	// Update floating ImGui windows
	if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}

	FrameIndex = (FrameIndex + 1) % Rendering::SwapChain::MAX_FRAMES_IN_FLIGHT;
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
	VkExtent2D extent{ Cast<uint32_t>(Window.GetWidth()), Cast<uint32_t>(Window.GetHeight()) };
	while (extent.width == 0 || extent.height == 0) {
		extent = { Cast<uint32_t>(Window.GetWidth()), Cast<uint32_t>(Window.GetHeight()) };
		glfwWaitEvents();
	}

	vkDeviceWaitIdle(Device.device());

	if (SwapChain == nullptr)
		SwapChain = std::make_unique<Rendering::SwapChain>(Device, extent);
	else {
		std::shared_ptr<Rendering::SwapChain> oldSwapChain = std::move(SwapChain);
		SwapChain = std::make_unique<Rendering::SwapChain>(Device, extent, oldSwapChain);
		if (!oldSwapChain->compareSwapFormats(*SwapChain.get()))
			PIT_ENGINE_FATAL(Log::Rendering, "Swapchain image/depth format has changed");
	}
	Engine::OnWindowResizeEvent.Invoke();

	MinImageCount = _GetMinImageCountFromPresentMode(SwapChain->GetPresentMode());
	//ImGui_ImplVulkan_SetMinImageCount(_GetMinImageCountFromPresentMode(SwapChain->GetPresentMode()));
}

void Renderer::_CreateCommandBuffers() {
	CommandBuffers.resize(Rendering::SwapChain::MAX_FRAMES_IN_FLIGHT);

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = Device.getCommandPool();
	allocInfo.commandBufferCount = static_cast<uint32_t>(CommandBuffers.size());

	if (vkAllocateCommandBuffers(Device.device(), &allocInfo, CommandBuffers.data()) != VK_SUCCESS)
		PIT_ENGINE_FATAL(Log::Rendering, "Failed to create commandBuffers!");
}

void Renderer::_FreeCommandBuffers() {
	vkFreeCommandBuffers(Device.device(), Device.getCommandPool(), Cast<uint32_t>(CommandBuffers.size()), CommandBuffers.data());
	CommandBuffers.clear();
}

void Renderer::_RecordCommandBuffer() {
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

	if (vkBeginCommandBuffer(CommandBuffers[FrameIndex], &beginInfo) != VK_SUCCESS)
		PIT_ENGINE_FATAL(Log::Rendering, "Failed to Begin commandBuffer!");

	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = SwapChain->getRenderPass();
	renderPassInfo.framebuffer = SwapChain->getFrameBuffer(ImageIndex);
	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = SwapChain->getSwapChainExtent();

	VkClearValue clearValues[2];
	clearValues[0].color = { .1f, .1f, .1f, 1.f };
	clearValues[1].depthStencil = { 1.f, 0 };
	renderPassInfo.clearValueCount = 2;
	renderPassInfo.pClearValues = clearValues;

	vkCmdBeginRenderPass(CommandBuffers[FrameIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

	VkViewport viewport{};
	viewport.x = viewport.y = 0;
	viewport.width = Cast<float>(SwapChain->getSwapChainExtent().width);
	viewport.height = Cast<float>(SwapChain->getSwapChainExtent().height);
	viewport.minDepth = 0.f;
	viewport.maxDepth = 1.f;
	VkRect2D scissors{ {0, 0}, SwapChain->getSwapChainExtent() };
	vkCmdSetViewport(CommandBuffers[FrameIndex], 0, 1, &viewport);
	vkCmdSetScissor(CommandBuffers[FrameIndex], 0, 1, &scissors);

	Engine::RenderEvent.Invoke();


	Engine::Rendering()->UIRenderer->Render(CommandBuffers[FrameIndex]);

	vkCmdEndRenderPass(CommandBuffers[FrameIndex]);
	if (vkEndCommandBuffer(CommandBuffers[FrameIndex]) != VK_SUCCESS)
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
	if (vkCreateDescriptorPool(Device.device(), &pool_info, nullptr, &DescriptorPool) != VK_SUCCESS)
		PIT_ENGINE_FATAL(Log::Rendering, "Failed to create descriptorPool!");
}


std::unique_ptr<Mesh> createCubeModel(Device& device, glm::vec3 offset) {
	Vertex::Builder builder;
	builder.vertices = {
		// left face (white)
		{{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
		{{-.5f, .5f, .5f}, {.9f, .9f, .9f}},
		{{-.5f, -.5f, .5f}, {.9f, .9f, .9f}},
		{{-.5f, .5f, -.5f}, {.9f, .9f, .9f}},

		// right face (yellow)
		{{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
		{{.5f, .5f, .5f}, {.8f, .8f, .1f}},
		{{.5f, -.5f, .5f}, {.8f, .8f, .1f}},
		{{.5f, .5f, -.5f}, {.8f, .8f, .1f}},

		// top face (orange, remember y axis points down)
		{{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
		{{.5f, -.5f, .5f}, {.9f, .6f, .1f}},
		{{-.5f, -.5f, .5f}, {.9f, .6f, .1f}},
		{{.5f, -.5f, -.5f}, {.9f, .6f, .1f}},

		// bottom face (red)
		{{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
		{{.5f, .5f, .5f}, {.8f, .1f, .1f}},
		{{-.5f, .5f, .5f}, {.8f, .1f, .1f}},
		{{.5f, .5f, -.5f}, {.8f, .1f, .1f}},

		// nose face (blue)
		{{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
		{{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
		{{-.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
		{{.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},

		// tail face (green)
		{{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
		{{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
		{{-.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
		{{.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
	};
	for (auto& v : builder.vertices)
		v.position += offset;
	
	builder.indices = { 0, 1, 2, 0, 3, 1, 4, 5, 6, 4, 7, 5, 8, 9, 10, 8, 11, 9,
					   12, 13, 14, 12, 15, 13, 16, 17, 18, 16, 19, 17, 20, 21, 22, 20, 23, 21 };
	return std::make_unique<Mesh>(device, builder);
}

void Renderer::_LoadModels() {	
	m_TestMesh = createCubeModel(Device, {0, 0, 0});
}