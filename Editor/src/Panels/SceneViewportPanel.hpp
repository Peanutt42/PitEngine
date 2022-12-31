#pragma once

#include "Core/CoreInclude.hpp"
#include "Rendering/Texture.hpp"
#include "EditorApplication.hpp"
#include "EditorWindowPanel.hpp"

namespace Pit::Editor {
	class SceneViewportPanel : public EditorWindowPanel {
	public:

		virtual void OnCreate() override;
		virtual void OnDestroy() override;

		virtual void OnGui() override;
	};
}