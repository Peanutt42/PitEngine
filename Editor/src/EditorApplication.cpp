#include "pch.hpp"
#include "EditorApplication.hpp"
#include "Panels/HierachyPanel.hpp"
#include "Panels/PropertiesPanel.hpp"
#include "Panels/SceneViewportPanel.hpp"
#include "Panels/ProfilerPanel.hpp"
#include "Panels/ContentBrowserPanel.hpp"
#include "EditorDockspace.hpp"

using namespace Pit;
using namespace Editor;

EditorAssetManager EditorApplication::s_AssetManager;
Array<EditorWindowPanel*> EditorApplication::s_WindowPanels;
Array<bool> EditorApplication::s_PanelKeyShortcutsPressed;


void EditorApplication::Init() {
	s_AssetManager.Init();

	s_WindowPanels.reserve(5);
	s_WindowPanels.push_back(new HierachyPanel());
	s_WindowPanels.push_back(new PropertiesPanel());
	s_WindowPanels.push_back(new SceneViewportPanel());
	s_WindowPanels.push_back(new ProfilerPanel());
	s_WindowPanels.push_back(new ContentBrowserPanel());

	Engine::UIRenderEvent += []() {
		if (ImGui::BeginMenuBar()) {
			if (ImGui::BeginMenu("File")) {
				if (ImGui::MenuItem("Exit"))
					Pit::Engine::ForceShutdown();

				ImGui::EndMenu();
			}
			MenubarCallback();
			ImGui::EndMenuBar();
		}
	};

	for (auto* layer : s_WindowPanels) {
		layer->OnCreate();
		Engine::UIRenderEvent += [=]() { layer->OnGUI(); };
	}
}

void EditorApplication::Shutdown() {
	for (auto* layer : s_WindowPanels)
		delete layer;
	s_WindowPanels.clear();

	s_AssetManager.Shutdown();
}

void EditorApplication::Update() {
	s_PanelKeyShortcutsPressed.resize(s_WindowPanels.size());
	for (int i = 0; i < s_WindowPanels.size(); i++) {
		bool pressed = s_WindowPanels[i]->Shortcut.size() > 0;
		for (auto keycode : s_WindowPanels[i]->Shortcut) {
			if (!Input::IsKeyDown(keycode))
				pressed = false;
		}
		s_PanelKeyShortcutsPressed[i] = pressed;
	}

	if (Input::IsKeyDown(Space)) {
		test1();
	}
}

void EditorApplication::MenubarCallback() {
	Array<bool> openWindows(s_WindowPanels.size());
	if (ImGui::BeginMenu("Windows")) {
		for (int i = 0; i < s_WindowPanels.size(); i++) {
			if (ImGui::MenuItem(s_WindowPanels[i]->Name.c_str(), "wip", nullptr, s_WindowPanels[i]->Enabled))
				openWindows[i] = true;
		}
		ImGui::EndMenu();
	}
	for (int i = 0; i < s_WindowPanels.size(); i++) {
		if (openWindows[i] || s_PanelKeyShortcutsPressed[i])
			s_WindowPanels[i]->Opened = true;
	}
}