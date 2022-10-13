#pragma once

#include "Engine/Main/Engine.hpp"

namespace Pit::Editor {
	class EditorApplication {
	public:
		EditorApplication(Engine& engine);
		~EditorApplication();

		void Init(), Shutdown();

		void Update();

	private:
		Engine& m_Engine;
		std::vector<Pit::Rendering::UI::Layer*> m_Panels;
		std::vector<bool> m_PanelKeyShortcutsPressed;
	};
}