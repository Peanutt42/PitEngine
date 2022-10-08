#include "Renderer.hpp"

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

    m_ImGuiRenderer = new ImGuiRenderer(m_Window->GetWindowHandle(), m_Instance, m_PhysicalDevice, m_Device, m_QueueFamily, m_Queue,
                                        m_PipelineCache, m_DescriptorPool, m_MinImageCount, m_MainWindowData.ImageCount, m_Allocator, &m_MainWindowData);
}

Renderer::~Renderer() {
    check_vk_result(vkDeviceWaitIdle(m_Device));

    delete m_ImGuiRenderer;
    _CleanupVulkanWindow();
    _CleanupVulkan();
	delete m_Window;
}

void Renderer::Update() {
	Rendering::Window::UpdateAllWindows();
	if (!m_Window->IsMinimized()) {
		
        static bool show_demo_window = true;
        static bool show_another_window = false;
        static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

        if (m_SwapChainRebuild) {
            int width, height;
            glfwGetFramebufferSize(m_Window->GetWindowHandle(), &width, &height);
            if (width > 0 && height > 0) {
                ImGui_ImplVulkan_SetMinImageCount(m_MinImageCount);
                ImGui_ImplVulkanH_CreateOrResizeWindow(m_Instance, m_PhysicalDevice, m_Device, &m_MainWindowData, m_QueueFamily, m_Allocator, width, height, m_MinImageCount);
                m_MainWindowData.FrameIndex = 0;
                m_SwapChainRebuild = false;
            }
        }

        m_ImGuiRenderer->BeginFrame();

        // Draw UI
        m_ImGuiRenderer->TestWindows();

        m_ImGuiRenderer->EndFrame();

        const bool is_minimized = ImGui::GetDrawData()->DisplaySize.x <= 0 ||
                                  ImGui::GetDrawData()->DisplaySize.y <= 0;
        if (!is_minimized) {
            m_MainWindowData.ClearValue.color.float32[0] = clear_color.x * clear_color.w;
            m_MainWindowData.ClearValue.color.float32[1] = clear_color.y * clear_color.w;
            m_MainWindowData.ClearValue.color.float32[2] = clear_color.z * clear_color.w;
            m_MainWindowData.ClearValue.color.float32[3] = clear_color.w;
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


void Renderer::_SetupVulkanWindow(VkSurfaceKHR surface) {
    int w, h;
    glfwGetFramebufferSize(m_Window->GetWindowHandle(), &w, &h);
    m_MainWindowData.Surface = surface;

    // Check for WSI support
    VkBool32 res;
    vkGetPhysicalDeviceSurfaceSupportKHR(m_PhysicalDevice, m_QueueFamily, m_MainWindowData.Surface, &res);
    if (res != VK_TRUE) {
        PIT_ENGINE_INFO(Log::Rendering, "Error no WSI support on physical device 0");
        exit(-1);
    }

    // Select Surface Format
    const VkFormat requestSurfaceImageFormat[] = { VK_FORMAT_B8G8R8A8_UNORM, VK_FORMAT_R8G8B8A8_UNORM, VK_FORMAT_B8G8R8_UNORM, VK_FORMAT_R8G8B8_UNORM };
    const VkColorSpaceKHR requestSurfaceColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
    m_MainWindowData.SurfaceFormat = ImGui_ImplVulkanH_SelectSurfaceFormat(m_PhysicalDevice, m_MainWindowData.Surface, requestSurfaceImageFormat, (size_t)IM_ARRAYSIZE(requestSurfaceImageFormat), requestSurfaceColorSpace);

    // Select Present Mode
    std::vector<VkPresentModeKHR> presentModes;
    if (UseVsync)
        presentModes.push_back(VK_PRESENT_MODE_FIFO_KHR);
    else {
        presentModes.push_back(VK_PRESENT_MODE_MAILBOX_KHR);
        presentModes.push_back(VK_PRESENT_MODE_IMMEDIATE_KHR);
        presentModes.push_back(VK_PRESENT_MODE_FIFO_KHR);
    }

    m_MainWindowData.PresentMode = ImGui_ImplVulkanH_SelectPresentMode(m_PhysicalDevice, m_MainWindowData.Surface, &presentModes[0], static_cast<int>(presentModes.size()));
    PIT_ENGINE_INFO(Log::Rendering, "[Vulkan] Selected PresentMode = {}", PresentModeToString(m_MainWindowData.PresentMode));

    IM_ASSERT(m_MinImageCount >= 2);
    ImGui_ImplVulkanH_CreateOrResizeWindow(m_Instance, m_PhysicalDevice, m_Device, &m_MainWindowData, m_QueueFamily, m_Allocator, w, h, m_MinImageCount);
}

void Renderer::_CleanupVulkanWindow() {
    ImGui_ImplVulkanH_DestroyWindow(m_Instance, m_Device, &m_MainWindowData, m_Allocator);
}


void Renderer::_FrameRender() {
    VkResult err;

    VkSemaphore image_acquired_semaphore = m_MainWindowData.FrameSemaphores[m_MainWindowData.SemaphoreIndex].ImageAcquiredSemaphore;
    VkSemaphore render_complete_semaphore = m_MainWindowData.FrameSemaphores[m_MainWindowData.SemaphoreIndex].RenderCompleteSemaphore;
    err = vkAcquireNextImageKHR(m_Device, m_MainWindowData.Swapchain, UINT64_MAX, image_acquired_semaphore, VK_NULL_HANDLE, &m_MainWindowData.FrameIndex);
    if (err == VK_ERROR_OUT_OF_DATE_KHR || err == VK_SUBOPTIMAL_KHR) {
        m_SwapChainRebuild = true;
        return;
    }
    check_vk_result(err);

    auto fd = m_ImGuiRenderer->PresentFrame();

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
    VkSemaphore render_complete_semaphore = m_MainWindowData.FrameSemaphores[m_MainWindowData.SemaphoreIndex].RenderCompleteSemaphore;
    VkPresentInfoKHR info = {};
    info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    info.waitSemaphoreCount = 1;
    info.pWaitSemaphores = &render_complete_semaphore;
    info.swapchainCount = 1;
    info.pSwapchains = &m_MainWindowData.Swapchain;
    info.pImageIndices = &m_MainWindowData.FrameIndex;
    VkResult err = vkQueuePresentKHR(m_Queue, &info);
    if (err == VK_ERROR_OUT_OF_DATE_KHR || err == VK_SUBOPTIMAL_KHR) {
        m_SwapChainRebuild = true;
        return;
    }
    check_vk_result(err);
    m_MainWindowData.SemaphoreIndex = (m_MainWindowData.SemaphoreIndex + 1) % m_MainWindowData.ImageCount; // Now we can use the next set of semaphores
}