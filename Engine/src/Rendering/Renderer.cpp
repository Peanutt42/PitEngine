#include "pch.hpp"
#include "Core/Engine.hpp"
#include "Renderer.hpp"
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include "Core/Engine.hpp"

using namespace Pit::Rendering;

DEFINE_EXTERN_PROFILE_STAT_FLOAT(RenderingRender, Rendering);
DEFINE_EXTERN_PROFILE_STAT_FLOAT(RenderingPresent, Rendering);
Renderer::Renderer()
	: Window(Pit::Engine::GetInfo().WindowName, 800, 600),
	Device(Window),
#define POOL_SIZE 500 // Oversize, but if too low can cause weird and hard to find bugs
	GlobalPool(DescriptorPool::Builder(Device)
			   .setMaxSets(POOL_SIZE)
			   .addPoolSize(VK_DESCRIPTOR_TYPE_SAMPLER, POOL_SIZE)
			   .addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, POOL_SIZE)
			   .addPoolSize(VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, POOL_SIZE)
			   .addPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, POOL_SIZE)
			   .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, POOL_SIZE)
			   .addPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, POOL_SIZE)
			   .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, POOL_SIZE)
			   .addPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, POOL_SIZE)
			   .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, POOL_SIZE)
			   .addPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, POOL_SIZE)
			   .addPoolSize(VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, POOL_SIZE)
			   .build()),
	GlobalSetLayout(DescriptorSetLayout::Builder(Device)
					.addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
					.build()) {

	SetGLFWWindowIcon(Window.GetWindowHandle(), FileSystem::GetEngineDir() + "assets/Icons/PitEngineLogo.png");
	_LoadModels();
	_RecreateSwapChain();
	_CreateCommandBuffers();

	for (auto& uboBuffer : UBOBuffers) {
		uboBuffer = std::make_unique<Buffer>(Device,
											 sizeof(GlobalUBO),
											 SwapChain::MAX_FRAMES_IN_FLIGHT,
											 VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
											 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
											 Device.properties.limits.minUniformBufferOffsetAlignment);
		uboBuffer->map();
	}

	for (int i = 0; i < GlobalDescriptorSets.size(); i++) {
		auto bufferInfo = UBOBuffers[i]->descriptorInfo();
		DescriptorWriter(*GlobalSetLayout, *GlobalPool)
			.writeBuffer(0, &bufferInfo)
			.build(GlobalDescriptorSets[i]);
	}
}

Renderer::~Renderer() {
	vkDeviceWaitIdle(Device.device());
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

	// Update Data
	GlobalUBO ubo{};
	ubo.projectionView = Engine::Rendering()->CurrentCamera->GetProjection() * Engine::Rendering()->CurrentCamera->GetView();
	UBOBuffers[FrameIndex]->writeToBuffer(&ubo);
	UBOBuffers[FrameIndex]->flush();

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
	VkExtent2D extent{ Cast<uint32>(Window.GetWidth()), Cast<uint32>(Window.GetHeight()) };
	while (extent.width == 0 || extent.height == 0) {
		extent = { Cast<uint32>(Window.GetWidth()), Cast<uint32>(Window.GetHeight()) };
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

	VkCommandBufferAllocateInfo allocInfo {
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		.pNext = nullptr,
		.commandPool = Device.getCommandPool(),
		.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		.commandBufferCount = static_cast<uint32>(CommandBuffers.size())
	};

	if (vkAllocateCommandBuffers(Device.device(), &allocInfo, CommandBuffers.data()) != VK_SUCCESS)
		PIT_ENGINE_FATAL(Log::Rendering, "Failed to create commandBuffers!");
}

void Renderer::_FreeCommandBuffers() {
	vkFreeCommandBuffers(Device.device(), Device.getCommandPool(), Cast<uint32>(CommandBuffers.size()), CommandBuffers.data());
	CommandBuffers.clear();
}

void Renderer::_RecordCommandBuffer() {
	VkCommandBufferBeginInfo beginInfo {
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO
	};

	if (vkBeginCommandBuffer(CommandBuffers[FrameIndex], &beginInfo) != VK_SUCCESS)
		PIT_ENGINE_FATAL(Log::Rendering, "Failed to Begin commandBuffer!");

	VkClearValue clearValues[2];
	clearValues[0].color = { .1f, .1f, .1f, 1.f };
	clearValues[1].depthStencil = { 1.f, 0 };
	VkRenderPassBeginInfo renderPassInfo{
		.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
		.pNext = nullptr,
		.renderPass = SwapChain->getRenderPass(),
		.framebuffer = SwapChain->getFrameBuffer(ImageIndex),
		.renderArea = {{0, 0}, SwapChain->getSwapChainExtent()},
		.clearValueCount = 2,
		.pClearValues = clearValues
	};

	vkCmdBeginRenderPass(CommandBuffers[FrameIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

	VkViewport viewport{
		.x = 0,
		.y = 0,
		.width = Cast<float>(SwapChain->getSwapChainExtent().width),
		.height = Cast<float>(SwapChain->getSwapChainExtent().height),
		.minDepth = 0.f,
		.maxDepth = 1.f
	};
	VkRect2D scissors{ {0, 0}, SwapChain->getSwapChainExtent() };
	vkCmdSetViewport(CommandBuffers[FrameIndex], 0, 1, &viewport);
	vkCmdSetScissor(CommandBuffers[FrameIndex], 0, 1, &scissors);

	Engine::RenderEvent.Invoke();


	Engine::Rendering()->UIRenderer->Render(CommandBuffers[FrameIndex]);

	vkCmdEndRenderPass(CommandBuffers[FrameIndex]);
	if (vkEndCommandBuffer(CommandBuffers[FrameIndex]) != VK_SUCCESS)
		PIT_ENGINE_ERR(Log::Rendering, "Failed to record commandBuffer!");
}

void Renderer::_LoadModels() {
	m_VaseMesh = Rendering::Mesh::CreateMeshFromFile(Device, FileSystem::GetSandboxDir() + "assets/models/smooth_vase.obj");
	m_QuadMesh = Rendering::Mesh::CreateMeshFromFile(Device, FileSystem::GetSandboxDir() + "assets/models/quad.obj");
}
