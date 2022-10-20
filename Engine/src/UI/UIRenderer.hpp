#pragma once

#include "Main/CoreInclude.hpp"
#include "UILayer.hpp"
#include "UILayerManager.hpp"
#include "Rendering/Renderer.hpp"
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_vulkan.h>
#include <vulkan/vulkan.h>

DECLARE_EXTERN_PROFILE_STAT_FLOAT(UIRenderingRecord, UIRendering, "UIRendering-Record");
DECLARE_EXTERN_PROFILE_STAT_FLOAT(UIRenderingRender, UIRendering, "UIRendering-Render");

namespace Pit::UI {
	class Renderer {
	public:
		Renderer(Rendering::Renderer* renderer);
		~Renderer();

		void Record();

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