#include "CoreInclude.hpp"
#include "Renderer.hpp"
#include "UI/UIRenderer.hpp"
#include "Main/Engine.hpp"

using namespace Pit::Rendering;



static void check_vk_result(VkResult err) {
	if (err == 0) return;

	PIT_ENGINE_ERR(Log::Rendering, "[Vulkan] VkResult: %d", err);

	if (err < 0) abort();
}

static void glfw_error_callback(int error, const char* description) {
	PIT_ENGINE_ERR(Log::Rendering, "[Glfw] {0:d} {1:s}", error, description);
}

#if VULKAN_DEBUG_MESSENGER
static VKAPI_ATTR VkBool32 VKAPI_CALL debugCall(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType, uint64_t object, size_t location, int32_t messageCode, const char* pLayerPrefix, const char* pMessage, void* pUserData) {
	(void)flags; (void)object; (void)location; (void)messageCode; (void)pUserData; (void)pLayerPrefix; // Unused arguments
	PIT_ENGINE_ERR(Log::Rendering, "[Vulkan]: {}", pMessage);
	return VK_FALSE;
}
#endif



Renderer::Renderer() {
	m_Window = new Window("PitEngine - Window", 800, 600);
	glfwSetErrorCallback(glfw_error_callback);
	if (!glfwVulkanSupported())
		PIT_ENGINE_ERR(Log::Rendering, "GLFW: Vulkan Not Supported");

	_SetupVulkan();

	_CreateWindowSurface();
	
	_SetupVulkanWindow(m_Surface);
}

Renderer::~Renderer() {
	check_vk_result(vkDeviceWaitIdle(m_Device));
	_CleanupVulkanWindow();
	_CleanupVulkan();
	delete m_Window;
}

void Renderer::Update() {
	Rendering::Window::UpdateAllWindows();
	if (!m_Window->IsMinimized()) {
		
		static bool show_demo_window = true;
		static bool show_another_window = false;
		static ImVec4 clear_color = ImVec4(0.1f, 0.1f, 0.1f, 1.f);

		if (m_SwapChainRebuild) {
			int width, height;
			glfwGetFramebufferSize(m_Window->GetWindowHandle(), &width, &height);
			if (width > 0 && height > 0) {
				ImGui_ImplVulkan_SetMinImageCount(m_MinImageCount);
				_CreateSwapchain();
				_CreateCommandBuffer();
				m_FrameIndex = 0;
				m_SwapChainRebuild = false;
			}
		}

		Engine::Instance->UIRenderer->BeginFrame();
		Engine::Instance->UIRenderer->RenderLayers();
		Engine::Instance->UIRenderer->EndFrame();

		const bool is_minimized = ImGui::GetDrawData()->DisplaySize.x <= 0 ||
								  ImGui::GetDrawData()->DisplaySize.y <= 0;
		if (!is_minimized) {
			m_ClearValue.color.float32[0] = clear_color.x * clear_color.w;
			m_ClearValue.color.float32[1] = clear_color.y * clear_color.w;
			m_ClearValue.color.float32[2] = clear_color.z * clear_color.w;
			m_ClearValue.color.float32[3] = clear_color.w;
			_FrameRender();
			_FramePresent();
		}

		// Update floating windows
		if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}
	}
}

bool Renderer::ShouldClose() {
	return m_Window->ShouldClose();
}


void Renderer::_SetupVulkan() {
	_CreateVkInstance();
	_SelectGPU();
	_SelectGraphicsQueue();
	_CreateLogicalDevice();
	_CreateDescriptorPool();
}

void Renderer::_CleanupVulkan() {
	_DestroyDescriptorPool();

#if VULKAN_DEBUG_MESSENGER
	_DestroyDebugMessenger();
#endif

	_DestroyLogicalDevice();
}


void Renderer::_CreateVkInstance() {
	uint32_t extensions_count = 0;
	const char** extensions = glfwGetRequiredInstanceExtensions(&extensions_count);

	VkInstanceCreateInfo create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	create_info.enabledExtensionCount = extensions_count;
	create_info.ppEnabledExtensionNames = extensions;

#if VULKAN_DEBUG_MESSENGER
	// Enabling validation layers
	const char* layers[] = { "VK_LAYER_KHRONOS_validation" };
	create_info.enabledLayerCount = 1;
	create_info.ppEnabledLayerNames = layers;

	// Enable debug report extension (we need additional storage, so we duplicate the user array to add our new extension to it)
	const char** extensions_ext = (const char**)malloc(sizeof(const char*) * (extensions_count + 1));
	memcpy(extensions_ext, extensions, extensions_count * sizeof(const char*));
	extensions_ext[extensions_count] = "VK_EXT_debug_report";
	create_info.enabledExtensionCount = extensions_count + 1;
	create_info.ppEnabledExtensionNames = extensions_ext;

	// Create Vulkan Instance
	VkResult err = vkCreateInstance(&create_info, m_Allocator, &m_Instance);
	check_vk_result(err);
	free(extensions_ext);


	_CreateDebugMessenger();
#else
	VkResult err = vkCreateInstance(&create_info, m_Allocator, &m_Instance);
	check_vk_result(err);
#endif
}

void Renderer::_DestroyVkInstance() {
	vkDestroyInstance(m_Instance, m_Allocator);
}

void Renderer::_CreateWindowSurface() {
	check_vk_result(glfwCreateWindowSurface(m_Instance, m_Window->GetWindowHandle(), m_Allocator, &m_Surface));
}


#if VULKAN_DEBUG_MESSENGER
void Renderer::_CreateDebugMessenger() {
	// Get the function pointer (required for any extensions)
	auto vkCreateDebugReportCallbackEXT = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(m_Instance, "vkCreateDebugReportCallbackEXT");
	IM_ASSERT(vkCreateDebugReportCallbackEXT != NULL);

	// Setup the debug report callback
	VkDebugReportCallbackCreateInfoEXT debug_report_ci = {};
	debug_report_ci.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
	debug_report_ci.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT;
	debug_report_ci.pfnCallback = debugCall;
	debug_report_ci.pUserData = NULL;
	VkResult err = vkCreateDebugReportCallbackEXT(m_Instance, &debug_report_ci, m_Allocator, &m_DebugMessenger);
	check_vk_result(err);
}

void Renderer::_DestroyDebugMessenger() {
	// Remove the debug report callback
	auto vkDestroyDebugReportCallbackEXT = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(m_Instance, "vkDestroyDebugReportCallbackEXT");
	vkDestroyDebugReportCallbackEXT(m_Instance, m_DebugMessenger, m_Allocator);
}
#endif


void Renderer::_SelectGPU() {
	uint32_t gpu_count;
	VkResult err = vkEnumeratePhysicalDevices(m_Instance, &gpu_count, NULL);
	check_vk_result(err);
	IM_ASSERT(gpu_count > 0);

	VkPhysicalDevice* gpus = (VkPhysicalDevice*)malloc(sizeof(VkPhysicalDevice) * gpu_count);
	err = vkEnumeratePhysicalDevices(m_Instance, &gpu_count, gpus);
	check_vk_result(err);

	// If a number >1 of GPUs got reported, find discrete GPU if present, or use first one available. This covers
	// most common cases (multi-gpu/integrated+dedicated graphics). Handling more complicated setups (multiple
	// dedicated GPUs) is out of scope of this sample.
	int use_gpu = 0;
	uint32_t highestMemory = 0;
	for (int i = 0; i < (int)gpu_count; i++) {
		VkPhysicalDeviceProperties properties;
		vkGetPhysicalDeviceProperties(gpus[i], &properties);
		if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
			if (properties.limits.maxMemoryAllocationCount > highestMemory) {
				properties.limits.maxMemoryAllocationCount = highestMemory;
				use_gpu = i;
			}
			break;
		}
	}
#if PIT_ENGINE_DEBUG || PIT_ENGINE_DEVELOPMENT
	VkPhysicalDeviceProperties properties;
	vkGetPhysicalDeviceProperties(gpus[use_gpu], &properties);
	PIT_ENGINE_INFO(Log::Rendering, "Chosen GPU: '{}'", properties.deviceName);
	PIT_ENGINE_INFO(Log::Rendering, "\tType: {}", PhysicalDeviceTypeToString(properties.deviceType));
	PIT_ENGINE_INFO(Log::Rendering, "\tMemory: {}MB", properties.limits.maxMemoryAllocationCount);
#endif
	m_PhysicalDevice = gpus[use_gpu];

	free(gpus);
}

void Renderer::_SelectGraphicsQueue() {
	uint32_t count;
	vkGetPhysicalDeviceQueueFamilyProperties(m_PhysicalDevice, &count, NULL);
	VkQueueFamilyProperties* queues = (VkQueueFamilyProperties*)malloc(sizeof(VkQueueFamilyProperties) * count);
	vkGetPhysicalDeviceQueueFamilyProperties(m_PhysicalDevice, &count, queues);
	for (uint32_t i = 0; i < count; i++)
		if (queues[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			m_QueueFamily = i;
			break;
		}
	free(queues);
	IM_ASSERT(m_QueueFamily != (uint32_t)-1);
}


void Renderer::_CreateLogicalDevice() {
	int device_extension_count = 1;
	const char* device_extensions[] = { "VK_KHR_swapchain" };
	const float queue_priority[] = { 1.0f };
	VkDeviceQueueCreateInfo queue_info[1] = {};
	queue_info[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queue_info[0].queueFamilyIndex = m_QueueFamily;
	queue_info[0].queueCount = 1;
	queue_info[0].pQueuePriorities = queue_priority;
	VkDeviceCreateInfo create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	create_info.queueCreateInfoCount = sizeof(queue_info) / sizeof(queue_info[0]);
	create_info.pQueueCreateInfos = queue_info;
	create_info.enabledExtensionCount = device_extension_count;
	create_info.ppEnabledExtensionNames = device_extensions;
	VkResult err = vkCreateDevice(m_PhysicalDevice, &create_info, m_Allocator, &m_Device);
	check_vk_result(err);
	vkGetDeviceQueue(m_Device, m_QueueFamily, 0, &m_Queue);
}

void Renderer::_DestroyLogicalDevice() {
	vkDestroyDevice(m_Device, m_Allocator);
}


void Renderer::_CreateDescriptorPool() {
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
	VkResult err = vkCreateDescriptorPool(m_Device, &pool_info, m_Allocator, &m_DescriptorPool);
	check_vk_result(err);
}

void Renderer::_DestroyDescriptorPool() {
	vkDestroyDescriptorPool(m_Device, m_DescriptorPool, m_Allocator);
}


static VkPresentModeKHR _SelectPresentMode(VkPhysicalDevice physical_device, VkSurfaceKHR surface, const VkPresentModeKHR* request_modes, int request_modes_count) {
	IM_ASSERT(request_modes != NULL);
	IM_ASSERT(request_modes_count > 0);

	// Request a certain mode and confirm that it is available. If not use VK_PRESENT_MODE_FIFO_KHR which is mandatory
	uint32_t avail_count = 0;
	vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &avail_count, NULL);
	ImVector<VkPresentModeKHR> avail_modes;
	avail_modes.resize((int)avail_count);
	vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &avail_count, avail_modes.Data);
	//for (uint32_t avail_i = 0; avail_i < avail_count; avail_i++)
	//    printf("[vulkan] avail_modes[%d] = %d\n", avail_i, avail_modes[avail_i]);

	for (int request_i = 0; request_i < request_modes_count; request_i++)
		for (uint32_t avail_i = 0; avail_i < avail_count; avail_i++)
			if (request_modes[request_i] == avail_modes[avail_i])
				return request_modes[request_i];

	return VK_PRESENT_MODE_FIFO_KHR; // Always available
}

static VkSurfaceFormatKHR _SelectSurfaceFormat(VkPhysicalDevice physical_device, VkSurfaceKHR surface, const VkFormat* request_formats, int request_formats_count, VkColorSpaceKHR request_color_space) {
	IM_ASSERT(request_formats != NULL);
	IM_ASSERT(request_formats_count > 0);

	// Per Spec Format and View Format are expected to be the same unless VK_IMAGE_CREATE_MUTABLE_BIT was set at image creation
	// Assuming that the default behavior is without setting this bit, there is no need for separate Swapchain image and image view format
	// Additionally several new color spaces were introduced with Vulkan Spec v1.0.40,
	// hence we must make sure that a format with the mostly available color space, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR, is found and used.
	uint32_t avail_count;
	vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &avail_count, NULL);
	ImVector<VkSurfaceFormatKHR> avail_format;
	avail_format.resize((int)avail_count);
	vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &avail_count, avail_format.Data);

	// First check if only one format, VK_FORMAT_UNDEFINED, is available, which would imply that any format is available
	if (avail_count == 1) {
		if (avail_format[0].format == VK_FORMAT_UNDEFINED) {
			VkSurfaceFormatKHR ret;
			ret.format = request_formats[0];
			ret.colorSpace = request_color_space;
			return ret;
		}
		else {
			// No point in searching another format
			return avail_format[0];
		}
	}
	else {
		// Request several formats, the first found will be used
		for (int request_i = 0; request_i < request_formats_count; request_i++)
			for (uint32_t avail_i = 0; avail_i < avail_count; avail_i++)
				if (avail_format[avail_i].format == request_formats[request_i] && avail_format[avail_i].colorSpace == request_color_space)
					return avail_format[avail_i];

		// If none of the requested image formats could be found, use the first available
		return avail_format[0];
	}
}

void Renderer::_SetupVulkanWindow(VkSurfaceKHR surface) {
	int w, h;
	glfwGetFramebufferSize(m_Window->GetWindowHandle(), &w, &h);
	m_Surface = surface;

	// Check for WSI support
	VkBool32 res;
	vkGetPhysicalDeviceSurfaceSupportKHR(m_PhysicalDevice, m_QueueFamily, m_Surface, &res);
	if (res != VK_TRUE) {
		PIT_ENGINE_INFO(Log::Rendering, "Error no WSI support on physical device 0");
		exit(-1);
	}

	// Select Surface Format
	const VkFormat requestSurfaceImageFormat[] = { VK_FORMAT_B8G8R8A8_UNORM, VK_FORMAT_R8G8B8A8_UNORM, VK_FORMAT_B8G8R8_UNORM, VK_FORMAT_R8G8B8_UNORM };
	const VkColorSpaceKHR requestSurfaceColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
	m_SurfaceFormat = _SelectSurfaceFormat(m_PhysicalDevice, m_Surface, requestSurfaceImageFormat, (size_t)IM_ARRAYSIZE(requestSurfaceImageFormat), requestSurfaceColorSpace);

	// Select Present Mode
	std::vector<VkPresentModeKHR> presentModes;
	if (UseVsync)
		presentModes.push_back(VK_PRESENT_MODE_FIFO_KHR);
	else {
		presentModes.push_back(VK_PRESENT_MODE_MAILBOX_KHR);
		presentModes.push_back(VK_PRESENT_MODE_IMMEDIATE_KHR);
		presentModes.push_back(VK_PRESENT_MODE_FIFO_KHR);
	}

	m_PresentMode = _SelectPresentMode(m_PhysicalDevice, m_Surface, &presentModes[0], static_cast<int>(presentModes.size()));
	PIT_ENGINE_INFO(Log::Rendering, "Selected PresentMode = {}", PresentModeToString(m_PresentMode));

	IM_ASSERT(m_MinImageCount >= 2);
	_CreateSwapchain();
	_CreateCommandBuffer();
}

void Renderer::_CleanupVulkanWindow() {
	vkDeviceWaitIdle(m_Device);

	_DestroyCommandBuffer();
	_DestroySwapchain();
	vkDestroyPipeline(m_Device, m_Pipeline, m_Allocator);
	vkDestroyRenderPass(m_Device, m_RenderPass, m_Allocator);
	vkDestroySurfaceKHR(m_Instance, m_Surface, m_Allocator);
}


void Renderer::_FrameRender() {
	VkResult err;

	VkSemaphore image_acquired_semaphore = m_FrameSemaphores[m_SemaphoreIndex].ImageAcquiredSemaphore;
	VkSemaphore render_complete_semaphore = m_FrameSemaphores[m_SemaphoreIndex].RenderCompleteSemaphore;
	err = vkAcquireNextImageKHR(m_Device, m_Swapchain, UINT64_MAX, image_acquired_semaphore, VK_NULL_HANDLE, &m_FrameIndex);
	if (err == VK_ERROR_OUT_OF_DATE_KHR || err == VK_SUBOPTIMAL_KHR) {
		m_SwapChainRebuild = true;
		return;
	}
	check_vk_result(err);

	auto fd = Engine::Instance->UIRenderer->PresentFrame();

	// Submit command buffer
	vkCmdEndRenderPass(fd->CommandBuffer);
	{
		VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		VkSubmitInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		info.waitSemaphoreCount = 1;
		info.pWaitSemaphores = &image_acquired_semaphore;
		info.pWaitDstStageMask = &wait_stage;
		info.commandBufferCount = 1;
		info.pCommandBuffers = &fd->CommandBuffer;
		info.signalSemaphoreCount = 1;
		info.pSignalSemaphores = &render_complete_semaphore;

		err = vkEndCommandBuffer(fd->CommandBuffer);
		check_vk_result(err);
		err = vkQueueSubmit(m_Queue, 1, &info, fd->Fence);
		check_vk_result(err);
	}
}

void Renderer::_FramePresent() {
	if (m_SwapChainRebuild)
		return;
	VkSemaphore render_complete_semaphore = m_FrameSemaphores[m_SemaphoreIndex].RenderCompleteSemaphore;
	VkPresentInfoKHR info = {};
	info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	info.waitSemaphoreCount = 1;
	info.pWaitSemaphores = &render_complete_semaphore;
	info.swapchainCount = 1;
	info.pSwapchains = &m_Swapchain;
	info.pImageIndices = &m_FrameIndex;
	VkResult err = vkQueuePresentKHR(m_Queue, &info);
	if (err == VK_ERROR_OUT_OF_DATE_KHR || err == VK_SUBOPTIMAL_KHR) {
		m_SwapChainRebuild = true;
		return;
	}
	check_vk_result(err);
	m_SemaphoreIndex = (m_SemaphoreIndex + 1) % m_ImageCount; // Now we can use the next set of semaphores
}


static int _GetMinImageCountFromPresentMode(VkPresentModeKHR present_mode) {
	if (present_mode == VK_PRESENT_MODE_MAILBOX_KHR)
		return 3;
	if (present_mode == VK_PRESENT_MODE_FIFO_KHR || present_mode == VK_PRESENT_MODE_FIFO_RELAXED_KHR)
		return 2;
	if (present_mode == VK_PRESENT_MODE_IMMEDIATE_KHR)
		return 1;
	IM_ASSERT(0);
	return 1;
}

void Renderer::_CreateSwapchain() {
	int width, height;
	glfwGetFramebufferSize(m_Window->GetWindowHandle(), &width, &height);
	VkResult err;
	VkSwapchainKHR old_swapchain = m_Swapchain;
	m_Swapchain = VK_NULL_HANDLE;
	err = vkDeviceWaitIdle(m_Device);
	check_vk_result(err);

	// We don't use ImGui_ImplVulkanH_DestroyWindow() because we want to preserve the old swapchain to create the new one.
	// Destroy old Framebuffer
	for (uint32_t i = 0; i < m_ImageCount; i++) {
		_DestroyFrame(m_Frames[i]);
		_DestroyFrameSemaphores(m_FrameSemaphores[i]);
	}
	IM_FREE(m_Frames);
	IM_FREE(m_FrameSemaphores);
	m_Frames = NULL;
	m_FrameSemaphores = NULL;
	m_ImageCount = 0;
	if (m_RenderPass)
		vkDestroyRenderPass(m_Device, m_RenderPass, m_Allocator);
	if (m_Pipeline)
		vkDestroyPipeline(m_Device, m_Pipeline, m_Allocator);

	// If min image count was not specified, request different count of images dependent on selected present mode
	if (m_MinImageCount == 0)
		m_MinImageCount = _GetMinImageCountFromPresentMode(m_PresentMode);

	// Create Swapchain
	{
		VkSwapchainCreateInfoKHR info = {};
		info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		info.surface = m_Surface;
		info.minImageCount = m_MinImageCount;
		info.imageFormat = m_SurfaceFormat.format;
		info.imageColorSpace = m_SurfaceFormat.colorSpace;
		info.imageArrayLayers = 1;
		info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;           // Assume that graphics family == present family
		info.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
		info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		info.presentMode = m_PresentMode;
		info.clipped = VK_TRUE;
		info.oldSwapchain = old_swapchain;
		VkSurfaceCapabilitiesKHR cap;
		err = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_PhysicalDevice, m_Surface, &cap);
		check_vk_result(err);
		if (info.minImageCount < cap.minImageCount)
			info.minImageCount = cap.minImageCount;
		else if (cap.maxImageCount != 0 && info.minImageCount > cap.maxImageCount)
			info.minImageCount = cap.maxImageCount;

		if (cap.currentExtent.width == 0xffffffff) {
			info.imageExtent.width = m_Width = width;
			info.imageExtent.height = m_Height = height;
		}
		else {
			info.imageExtent.width = m_Width = cap.currentExtent.width;
			info.imageExtent.height = m_Height = cap.currentExtent.height;
		}
		err = vkCreateSwapchainKHR(m_Device, &info, m_Allocator, &m_Swapchain);
		check_vk_result(err);
		err = vkGetSwapchainImagesKHR(m_Device, m_Swapchain, &m_ImageCount, NULL);
		check_vk_result(err);
		VkImage backbuffers[16] = {};
		IM_ASSERT(m_ImageCount >= m_MinImageCount);
		IM_ASSERT(m_ImageCount < IM_ARRAYSIZE(backbuffers));
		err = vkGetSwapchainImagesKHR(m_Device, m_Swapchain, &m_ImageCount, backbuffers);
		check_vk_result(err);

		IM_ASSERT(m_Frames == NULL);
		m_Frames = (Frame*)IM_ALLOC(sizeof(Frame) * m_ImageCount);
		m_FrameSemaphores = (FrameSemaphores*)IM_ALLOC(sizeof(FrameSemaphores) * m_ImageCount);
		memset(m_Frames, 0, sizeof(m_Frames[0]) * m_ImageCount);
		memset(m_FrameSemaphores, 0, sizeof(m_FrameSemaphores[0]) * m_ImageCount);
		for (uint32_t i = 0; i < m_ImageCount; i++)
			m_Frames[i].Backbuffer = backbuffers[i];
	}
	if (old_swapchain)
		vkDestroySwapchainKHR(m_Device, old_swapchain, m_Allocator);

	// Create the Render Pass
	{
		VkAttachmentDescription attachment = {};
		attachment.format = m_SurfaceFormat.format;
		attachment.samples = VK_SAMPLE_COUNT_1_BIT;
		attachment.loadOp = m_ClearEnable ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		VkAttachmentReference color_attachment = {};
		color_attachment.attachment = 0;
		color_attachment.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		VkSubpassDescription subpass = {};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &color_attachment;
		VkSubpassDependency dependency = {};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.srcAccessMask = 0;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		VkRenderPassCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		info.attachmentCount = 1;
		info.pAttachments = &attachment;
		info.subpassCount = 1;
		info.pSubpasses = &subpass;
		info.dependencyCount = 1;
		info.pDependencies = &dependency;
		err = vkCreateRenderPass(m_Device, &info, m_Allocator, &m_RenderPass);
		check_vk_result(err);

		// We do not create a pipeline by default as this is also used by examples' main.cpp,
		// but secondary viewport in multi-viewport mode may want to create one with:
		//ImGui_ImplVulkan_CreatePipeline(device, allocator, VK_NULL_HANDLE, wd->RenderPass, VK_SAMPLE_COUNT_1_BIT, &wd->Pipeline, bd->Subpass);
	}

	// Create The Image Views
	{
		VkImageViewCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		info.viewType = VK_IMAGE_VIEW_TYPE_2D;
		info.format = m_SurfaceFormat.format;
		info.components.r = VK_COMPONENT_SWIZZLE_R;
		info.components.g = VK_COMPONENT_SWIZZLE_G;
		info.components.b = VK_COMPONENT_SWIZZLE_B;
		info.components.a = VK_COMPONENT_SWIZZLE_A;
		VkImageSubresourceRange image_range = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
		info.subresourceRange = image_range;
		for (uint32_t i = 0; i < m_ImageCount; i++) {
			Frame& frame = m_Frames[i];
			info.image = frame.Backbuffer;
			err = vkCreateImageView(m_Device, &info, m_Allocator, &frame.BackbufferView);
			check_vk_result(err);
		}
	}

	// Create Framebuffer
	{
		VkImageView attachment[1];
		VkFramebufferCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		info.renderPass = m_RenderPass;
		info.attachmentCount = 1;
		info.pAttachments = attachment;
		info.width = m_Width;
		info.height = m_Height;
		info.layers = 1;
		for (uint32_t i = 0; i < m_ImageCount; i++) {
			Frame& frame = m_Frames[i];
			attachment[0] = frame.BackbufferView;
			err = vkCreateFramebuffer(m_Device, &info, m_Allocator, &frame.Framebuffer);
			check_vk_result(err);
		}
	}
}

void Renderer::_DestroySwapchain() {
	vkDestroySwapchainKHR(m_Device, m_Swapchain, m_Allocator);
}


void Renderer::_CreateCommandBuffer() {
	IM_ASSERT(m_PhysicalDevice != VK_NULL_HANDLE && m_Device != VK_NULL_HANDLE);
	(void)m_PhysicalDevice;
	(void)m_Allocator;

	// Create Command Buffers
	VkResult err;
	for (uint32_t i = 0; i < m_ImageCount; i++) {
		Frame& frame = m_Frames[i];
		FrameSemaphores& frameSemaphor = m_FrameSemaphores[i];
		{
			VkCommandPoolCreateInfo info = {};
			info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
			info.queueFamilyIndex = m_QueueFamily;
			err = vkCreateCommandPool(m_Device, &info, m_Allocator, &frame.CommandPool);
			check_vk_result(err);
		}
		{
			VkCommandBufferAllocateInfo info = {};
			info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			info.commandPool = frame.CommandPool;
			info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			info.commandBufferCount = 1;
			err = vkAllocateCommandBuffers(m_Device, &info, &frame.CommandBuffer);
			check_vk_result(err);
		}
		{
			VkFenceCreateInfo info = {};
			info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
			info.flags = VK_FENCE_CREATE_SIGNALED_BIT;
			err = vkCreateFence(m_Device, &info, m_Allocator, &frame.Fence);
			check_vk_result(err);
		}
		{
			VkSemaphoreCreateInfo info = {};
			info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
			err = vkCreateSemaphore(m_Device, &info, m_Allocator, &frameSemaphor.ImageAcquiredSemaphore);
			check_vk_result(err);
			err = vkCreateSemaphore(m_Device, &info, m_Allocator, &frameSemaphor.RenderCompleteSemaphore);
			check_vk_result(err);
		}
	}
}

void Renderer::_DestroyCommandBuffer() {
	for (uint32_t i = 0; i < m_ImageCount; i++) {
		_DestroyFrame(m_Frames[i]);
		_DestroyFrameSemaphores(m_FrameSemaphores[i]);
	}
	IM_FREE(m_Frames);
	IM_FREE(m_FrameSemaphores);
	m_Frames = NULL;
	m_FrameSemaphores = NULL;
}

void Renderer::_DestroyFrame(Frame& frame) {
	vkDestroyFence(m_Device, frame.Fence, m_Allocator);
	vkFreeCommandBuffers(m_Device, frame.CommandPool, 1, &frame.CommandBuffer);
	vkDestroyCommandPool(m_Device, frame.CommandPool, m_Allocator);
	frame.Fence = VK_NULL_HANDLE;
	frame.CommandBuffer = VK_NULL_HANDLE;
	frame.CommandPool = VK_NULL_HANDLE;

	vkDestroyImageView(m_Device, frame.BackbufferView, m_Allocator);
	vkDestroyFramebuffer(m_Device, frame.Framebuffer, m_Allocator);
}

void Renderer::_DestroyFrameSemaphores(FrameSemaphores& frameSemaphores) {
	vkDestroySemaphore(m_Device, frameSemaphores.ImageAcquiredSemaphore, m_Allocator);
	vkDestroySemaphore(m_Device, frameSemaphores.RenderCompleteSemaphore, m_Allocator);
	frameSemaphores.ImageAcquiredSemaphore = frameSemaphores.RenderCompleteSemaphore = VK_NULL_HANDLE;
}
