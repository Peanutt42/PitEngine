#pragma once

#include "Main/CoreInclude.hpp"
#include "UILayer.hpp"
#include "UILayerManager.hpp"
#include "Rendering/Renderer.hpp"
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_vulkan.h>
#include <vulkan/vulkan.h>


namespace Pit::Rendering::UI {
	class Renderer {
	public:
		Renderer(const RendererContext& context);
		~Renderer();

		void BeginFrame();
		void EndFrame();
		void DrawLayers();
		void Render(VkCommandBuffer commandBuffer);

		LayerManager* GetUILayerManager() { return m_UILayerManager; }

	private:
		VkDevice m_Device;
		LayerManager* m_UILayerManager;
	};
}