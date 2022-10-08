#pragma once

#include "Main/CoreInclude.hpp"
#include "Window.hpp"
#include "VulkanDebugMessenger.hpp"
#include "VulkanUtils.hpp"
#include "VulkanShaderLoader.hpp"
#include "VulkanTypesToString.hpp"
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_vulkan.h>
#include <vulkan/vulkan.h>
#include "ImGui/ImGuiRenderer.hpp"

namespace Pit::Rendering {
	class Renderer {
	public:
		Renderer();
		~Renderer();

		void Update();
		bool ShouldClose();

	private:
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

		ImGui_ImplVulkanH_Window m_MainWindowData;
		int                      m_MinImageCount = 2;
		bool                     m_SwapChainRebuild = false;

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

		ImGuiRenderer* m_ImGuiRenderer = nullptr;
	};
}