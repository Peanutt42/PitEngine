#pragma once

#include "pch.hpp"
#include "Rendering/UI/UILayer.hpp"

namespace Pit::Editor {
	class EditorWindowPanel : public Rendering::UI::Layer {
	public:
		virtual void OnCreate() = 0;
		virtual void OnDestroy() = 0;
		virtual void OnGui() = 0;

		// UILayer
		virtual void OnAttach() override {
			OnCreate();
		}
		virtual void OnDetach() override {
			OnDestroy();
		}
		virtual void OnGUI() override {
			if (!Opened) return;

			ImGui::Begin(Name.c_str(), &Opened);
			if (Enabled)
				OnGui();

			ImGui::End();
		}

		bool Opened = true;
		std::string Name = "Window";
		std::vector<KeyCode> Shortcut;
		bool Enabled = true;
	};
}