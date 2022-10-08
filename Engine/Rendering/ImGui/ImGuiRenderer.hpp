#pragma once

#include "Main/CoreInclude.hpp"
#include "ImGuiLayer.hpp"
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_vulkan.h>
#include <vulkan/vulkan.h>

namespace Pit::Rendering {
	class ImGuiRenderer {
	public:
		ImGuiRenderer(GLFWwindow* window, VkInstance instance, VkPhysicalDevice physicalDevice, VkDevice device,
					  uint32_t queueFamily, VkQueue queue, VkPipelineCache pipelineCache, VkDescriptorPool descriptorPool,
					  uint32_t minImageCount, uint32_t imageCount, VkAllocationCallbacks* allocator, ImGui_ImplVulkanH_Window* wd);
		~ImGuiRenderer();

		void BeginFrame();
		void EndFrame();
		ImGui_ImplVulkanH_Frame* PresentFrame();
		void TestWindows();

		template<typename T>
		void PushLayer() {
			static_assert(std::is_base_of<ImGuiLayer, T>::value, "Pushed type is not subclass of Layer!");
			m_LayerStack.emplace_back(std::make_shared<T>())->OnAttach();
		}

		void PushLayer(const std::shared_ptr<ImGuiLayer>& layer) { m_LayerStack.emplace_back(layer); layer->OnAttach(); }


	private:
		std::vector<std::shared_ptr<ImGuiLayer>> m_LayerStack;
		std::function<void()> m_MenubarCallback;

		ImGui_ImplVulkanH_Window* m_Wd;
		VkDevice m_Device;
	};
}