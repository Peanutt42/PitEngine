#pragma once

#include "Main/CoreInclude.hpp"
#include "Window.hpp"
#include "VulkanDebugMessenger.hpp"
#include "VulkanUtils.hpp"
#include "VulkanShaderLoader.hpp"
#include "VulkanTypesToString.hpp"
#include "VulkanFrame.hpp"
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_vulkan.h>
#include <vulkan/vulkan.h>


namespace Pit::Rendering {
	class Renderer {
	public:
		Renderer();
		~Renderer();

		void Update();
		bool ShouldClose();

		uint32_t GetWidth() { return m_Width; }
		uint32_t GetHeight() { return m_Height; }

	public:

		// Window
		Window* m_Window = nullptr;

		// Vulkan
#define VULKAN_DEBUG_MESSENGER true
		VkAllocationCallbacks*	 m_Allocator = NULL;
		VkInstance               m_Instance = VK_NULL_HANDLE;
		VkPhysicalDevice         m_PhysicalDevice = VK_NULL_HANDLE;
		VkDevice                 m_Device = VK_NULL_HANDLE;
		uint32_t                 m_QueueFamily = (uint32_t)-1;
		VkQueue                  m_Queue = VK_NULL_HANDLE;
		VkSurfaceKHR			 m_Surface;
#if VULKAN_DEBUG_MESSENGER
		VkDebugReportCallbackEXT m_DebugMessenger = VK_NULL_HANDLE;
#endif
		VkPipelineCache          m_PipelineCache = VK_NULL_HANDLE;
		VkDescriptorPool         m_DescriptorPool = VK_NULL_HANDLE;

		uint32_t                 m_Width;
		uint32_t                 m_Height;
		VkSwapchainKHR      m_Swapchain;
		VkSurfaceFormatKHR  m_SurfaceFormat;
		VkPresentModeKHR    m_PresentMode;
		VkRenderPass        m_RenderPass;
		VkPipeline          m_Pipeline;               // The window pipeline may uses a different VkRenderPass than the one passed in ImGui_ImplVulkan_InitInfo
		bool                m_ClearEnable;
		VkClearValue        m_ClearValue;
		uint32_t            m_FrameIndex;             // Current frame being rendered to (0 <= FrameIndex < FrameInFlightCount)
		uint32_t            m_ImageCount;             // Number of simultaneous in-flight frames (returned by vkGetSwapchainImagesKHR, usually derived from min_image_count)
		uint32_t            m_SemaphoreIndex;         // Current set of swapchain wait semaphores we're using (needs to be distinct from per frame data)
		Frame* m_Frames;
		FrameSemaphores* m_FrameSemaphores;

		uint32_t                      m_MinImageCount = 2;
		bool                     m_SwapChainRebuild = false;

	private:
		void _SetupVulkan(), _CleanupVulkan();
		void _CreateVkInstance(), _DestroyVkInstance();
		void _CreateWindowSurface();
#if VULKAN_DEBUG_MESSENGER
		void _CreateDebugMessenger(), _DestroyDebugMessenger();
#endif
		void _SelectGPU();
		void _SelectGraphicsQueue();
		void _CreateLogicalDevice(), _DestroyLogicalDevice();
		void _CreateDescriptorPool(), _DestroyDescriptorPool();
		void _SetupVulkanWindow(VkSurfaceKHR surface), _CleanupVulkanWindow();
		void _FrameRender();
		void _FramePresent();

		void _CreateSwapchain(), _DestroySwapchain();
		void _CreateCommandBuffer(), _DestroyCommandBuffer();

		void _DestroyFrame(Frame& frame);
		void _DestroyFrameSemaphores(FrameSemaphores& frameSemaphores);
	};
}