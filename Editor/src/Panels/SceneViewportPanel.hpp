#pragma once

#include "Core/CoreInclude.hpp"
#include "UI/UILayer.hpp"
#include "Rendering/Texture.hpp"
#include "EditorApplication.hpp"
#include "EditorWindowPanel.hpp"
#include <vulkan/vulkan.h>

namespace Pit::Editor {
	class SceneViewportPanel : public EditorWindowPanel {
	public:

		virtual void OnCreate() override;
		virtual void OnDestroy() override;

		virtual void OnGui() override;

	private:
		Pit::Rendering::Texture* m_SceneTexture;
	};
}