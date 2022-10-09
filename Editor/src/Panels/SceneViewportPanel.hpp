#pragma once

#include "Engine/Main/CoreInclude.hpp"
#include "Engine/Rendering/UI/UILayer.hpp"
#include "Engine/Rendering/VulkanTexture.hpp"
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