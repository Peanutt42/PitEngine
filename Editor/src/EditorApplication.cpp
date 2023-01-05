#include "pch.hpp"
#include "EditorApplication.hpp"
#include "Panels/HierachyPanel.hpp"
#include "Panels/PropertiesPanel.hpp"
#include "Panels/SceneViewportPanel.hpp"
#include "Panels/ProfilerPanel.hpp"
#include "Panels/ContentBrowserPanel.hpp"
#include "Panels/LoggingOutputPanel.hpp"
#include "Panels/BuildGamePanel.hpp"
#include "EditorDockspace.hpp"
#include "Scripting\ScriptingSubmodule.hpp"

using namespace Pit;
using namespace Editor;

EditorAssetManager EditorApplication::s_AssetManager;
Array<EditorWindowPanel*> EditorApplication::s_WindowPanels;
Array<bool> EditorApplication::s_PanelKeyShortcutsPressed;

ProjectInfo EditorApplication::s_CurrentProject{ .Name = "NULL", .EngineVersion = Version(0,0,0) };

std::filesystem::path EditorApplication::s_CurrentSceneFilepath;

void EditorApplication::SelectProject(const ProjectInfo& projectInfo) {
	s_CurrentProject = projectInfo;
}


// TODO: Temp
static void OpenSceneFromFile() {
	String filepath = FileDialogs::OpenFile("PitEngine Scene (*.pitscene)\0*.pitscene\0");
}
static void SaveSceneToFile() {
	String filepath = FileDialogs::SaveFile("PitEngine Scene (*.pitscene)\0*.pitscene\0");
}
static void SaveScene() {

}


void EditorApplication::Init() {
	PIT_ENGINE_ASSERT(Editor, s_CurrentProject.Name != "NULL", "Editor has no project selected!\n - add -proj MyProject to your console args");
	PIT_ENGINE_ASSERT(Editor, s_CurrentProject.EngineVersion == Engine::Version, "Selected Project has a diffrent version ({0}) than this editors version {1}", s_CurrentProject.EngineVersion, Engine::Version);

	s_AssetManager.Init();

	s_WindowPanels.reserve(7);
	s_WindowPanels.push_back(new HierachyPanel());
	s_WindowPanels.push_back(new PropertiesPanel());
	s_WindowPanels.push_back(new SceneViewportPanel());
	s_WindowPanels.push_back(new ProfilerPanel());
	s_WindowPanels.push_back(new ContentBrowserPanel());
	s_WindowPanels.push_back(new LoggingOutputPanel());
	s_WindowPanels.push_back(new BuildGamePanel());
	
	Engine::UIRenderEvent += []() {
		if (ImGui::BeginMenuBar()) {
			if (ImGui::BeginMenu("File")) {
				if (ImGui::MenuItem("Exit"))
					Pit::Engine::ForceShutdown();

				ImGui::EndMenu();
			}
			Array<bool> openWindows(s_WindowPanels.size());
			if (ImGui::BeginMenu("File")) {
				if (ImGui::MenuItem("Open File", "Ctrl + O"))
					OpenSceneFromFile();
				if (ImGui::MenuItem("Save", "Ctrl + S"))
					SaveScene();
				if (ImGui::MenuItem("Save to File", "Ctrl + Shift + S"))
					SaveSceneToFile();

				ImGui::EndMenu();
			}
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

			if (ImGui::BeginMenu("Script")) {
				if (ImGui::MenuItem("Reload Assembly", "Ctrl + R"))
					Engine::Scripting()->ReloadAssembly();

				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}
	};

	for (EditorWindowPanel* layer : s_WindowPanels) {
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

	if (Input::IsKeyDown(KeyCode::LeftControl) && !Input::IsKeyDown(KeyCode::LeftShift) && Input::IsKeyPressed(KeyCode::O))
		OpenSceneFromFile();

	if (Input::IsKeyDown(KeyCode::LeftControl) && Input::IsKeyReleased(KeyCode::S))
		SaveScene();
	if (Input::IsKeyDown(KeyCode::LeftControl) && Input::IsKeyDown(KeyCode::LeftShift) && Input::IsKeyReleased(KeyCode::S))
		SaveSceneToFile();

	if (Input::IsKeyDown(KeyCode::LeftControl) && Input::IsKeyPressed(KeyCode::R))
		Engine::Scripting()->ReloadAssembly();
}