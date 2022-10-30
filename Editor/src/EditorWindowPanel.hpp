#pragma once

#include "pch.hpp"
#include "UI/UILayer.hpp"

namespace Pit::Editor {
	class EditorWindowPanel : public UI::Layer {
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
		String Name = "Window";
		Array<KeyCode> Shortcut;
		bool Enabled = true;
	};
}