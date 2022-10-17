#pragma once

#include "Main/CoreInclude.hpp"
#include "UILayer.hpp"
#include "UILayerManager.hpp"
#include "Rendering/Renderer.hpp"
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_vulkan.h>
#include <vulkan/vulkan.h>


namespace Pit::UI {
	class Renderer {
	public:
		Renderer(Rendering::Renderer* renderer);
		~Renderer();

		void BeginFrame();
		void EndFrame();
		void DrawLayers();
		void Render(VkCommandBuffer commandBuffer);

		void SetMenubarCallback(std::function<void()> callback) { m_MenubarCallback = callback; }

		LayerManager* GetUILayerManager() { return m_UILayerManager; }

	private:
		VkDevice m_Device;
		LayerManager* m_UILayerManager;
		std::function<void()> m_MenubarCallback;
	};
}