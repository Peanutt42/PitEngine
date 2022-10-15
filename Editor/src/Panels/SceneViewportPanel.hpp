#pragma once

#include "Main/CoreInclude.hpp"
#include "Rendering/UI/UILayer.hpp"
#include "Rendering/Vulkan/VulkanTexture.hpp"
#include "EditorApplication.hpp"
#include <vulkan/vulkan.h>

namespace Pit::Editor {
	class SceneViewportPanel : public Rendering::UI::Layer {
	public:

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		virtual void OnGUI() override;

	private:
		Pit::Rendering::Texture* m_SceneTexture;
	};
}