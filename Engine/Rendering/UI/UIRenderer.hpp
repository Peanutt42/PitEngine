#pragma once

#include "Main/CoreInclude.hpp"
#include "UILayer.hpp"
#include "UILayerManager.hpp"
#include "Rendering/VulkanTypes.hpp"
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_vulkan.h>
#include <vulkan/vulkan.h>


namespace Pit::Rendering::UI {
	class Renderer {
	public:
		Renderer(GLFWwindow* window, VkInstance instance, VkPhysicalDevice physicalDevice, VkDevice device,
					  uint32_t queueFamily, VkQueue queue, VkPipelineCache pipelineCache, VkDescriptorPool descriptorPool,
					  uint32_t minImageCount, uint32_t imageCount, VkAllocationCallbacks* allocator);
		~Renderer();

		void BeginFrame();
		void EndFrame();
		Pit::Rendering::Frame* PresentFrame();
		void RenderLayers();

		LayerManager* GetUILayerManager() { return m_UILayerManager; }

	private:
		VkDevice m_Device;
		LayerManager* m_UILayerManager;
	};
}