#pragma once

#include "pch.hpp"
#include <imgui/imgui.h>

namespace Pit::Editor {
	class EditorWindowPanel {
	public:
		virtual void OnCreate() {}
		virtual void OnDestroy() {}
		virtual void OnGui() {}
		virtual void OnUpdate() {}

		void OnGUI() {
			if (!Opened) return;

			ImGui::Begin(Name.c_str(), &Opened);
			if (Enabled)
				OnGui();

			ImGui::End();
		}

		bool Opened = true;
		String Name = "Window";
		Array<KeyCode> Shortcut;
		bool Enabled = true;
	};
}