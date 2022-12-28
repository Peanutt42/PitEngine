#include "pch.hpp"
#include "EditorApplication.hpp"
#include "Panels/HierachyPanel.hpp"
#include "Panels/PropertiesPanel.hpp"
#include "Panels/SceneViewportPanel.hpp"
#include "Panels/ProfilerPanel.hpp"
#include "Panels/ContentBrowserPanel.hpp"
#include "Panels\LoggingOutputPanel.hpp"
#include "EditorDockspace.hpp"

using namespace Pit;
using namespace Editor;

EditorAssetManager EditorApplication::s_AssetManager;
Array<EditorWindowPanel*> EditorApplication::s_WindowPanels;
Array<bool> EditorApplication::s_PanelKeyShortcutsPressed;

ProjectInfo EditorApplication::s_CurrentProject{ .Name = "NULL", .EngineVersion = Version(0,0,0) };


void EditorApplication::SelectProject(const ProjectInfo& projectInfo) {
	s_CurrentProject = projectInfo;
}

void EditorApplication::Init() {
	PIT_ENGINE_ASSERT(Editor, s_CurrentProject.Name != "NULL", "Editor has no project selected!\n - add -proj MyProject to your console args");
	PIT_ENGINE_ASSERT(Editor, s_CurrentProject.EngineVersion == Engine::Version, "Selected Project has a diffrent version ({0}) than this editors version {1}", s_CurrentProject.EngineVersion, Engine::Version);

	s_AssetManager.Init();

	s_WindowPanels.reserve(5);
	s_WindowPanels.push_back(new HierachyPanel());
	s_WindowPanels.push_back(new PropertiesPanel());
	s_WindowPanels.push_back(new SceneViewportPanel());
	s_WindowPanels.push_back(new ProfilerPanel());
	s_WindowPanels.push_back(new ContentBrowserPanel());
	s_WindowPanels.push_back(new LoggingOutputPanel());
	
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
		Engine::UIRenderEvent += [layer]() { layer->OnGUI(); };
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
}

void EditorApplication::MenubarCallback() {
	Array<bool> openWindows(s_WindowPanels.size());
	if (ImGui::BeginMenu("Windows")) {
		for (int i = 0; i < s_WindowPanels.size(); i++) {
			String shortcutStr;
			for (int j = 0; j < s_WindowPanels[i]->Shortcut.size(); j++) {
				shortcutStr += KeyCodeToString(s_WindowPanels[i]->Shortcut[j]);
				if (j + 1 != s_WindowPanels[i]->Shortcut.size()) shortcutStr += " + ";
			}
			if (ImGui::MenuItem(s_WindowPanels[i]->Name.c_str(), shortcutStr.c_str(), nullptr, s_WindowPanels[i]->Enabled))
				openWindows[i] = true;
		}
		ImGui::EndMenu();
	}
	for (int i = 0; i < s_WindowPanels.size(); i++) {
		if (openWindows[i] || s_PanelKeyShortcutsPressed[i])
			s_WindowPanels[i]->Opened = true;
	}
}