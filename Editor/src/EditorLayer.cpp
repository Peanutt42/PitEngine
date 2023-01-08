#include "pch.hpp"
#include "EditorLayer.hpp"
#include "Rendering/RenderingSystem.hpp"
#include "Rendering/Renderer.hpp"
#include "Panels/HierachyPanel.hpp"
#include "Panels/PropertiesPanel.hpp"
#include "Panels/SceneViewportPanel.hpp"
#include "Panels/ProfilerPanel.hpp"
#include "Panels/ContentBrowserPanel.hpp"
#include "Panels/LoggingOutputPanel.hpp"
#include "Panels/BuildGamePanel.hpp"
#include "EditorDockspace.hpp"
#include "Scripting\ScriptingSubmodule.hpp"
#include "ECS/ECSSceneSerializer.hpp"
#include "ImGuiThemes.hpp"

namespace Pit::Editor {
	EditorLayer::EditorLayer() : Layer("Editor") {
		if (s_Instance) { delete this; return; }
		s_Instance = this;
		
		String projDir;
		auto& consoleArgs = Engine::GetSettings().ConsoleArgs;
		for (int i = 0; i < consoleArgs.size(); i++) {
			if (consoleArgs[i] == "-proj" && consoleArgs.size() < i + 1) projDir = consoleArgs[i + 1];
		}
		if (projDir.empty()) projDir = FileDialogs::OpenFile("PitEngine Project (*.pitproj)\0*.pitproj\0");

		if (!std::filesystem::exists(projDir)) std::exit(0);

		YAML::Node in = YAML::LoadFile(projDir);
		String projName = in["Name"].as<String>();
		String projEngineVersionStr = in["EngineVersion"].as<String>();
		m_CurrentProject = ProjectInfo{ .Name = projName, .EngineVersion = Version(projEngineVersionStr) };
		PIT_ENGINE_ASSERT(Editor, m_CurrentProject.EngineVersion == Engine::Version, "Project {0:s}'s version ({1}) is not compatible with this version of the Editor({2})", projName, m_CurrentProject.EngineVersion, Engine::Version);
	}

	EditorLayer::~EditorLayer() {
		s_Instance = nullptr;
	}
	
	void EditorLayer::OnCreate() {
		m_AssetManager.Init();

		m_WindowPanels.reserve(7);
		m_WindowPanels.push_back(new HierachyPanel());
		m_WindowPanels.push_back(new PropertiesPanel());
		m_WindowPanels.push_back(new SceneViewportPanel());
		m_WindowPanels.push_back(new ProfilerPanel());
		m_WindowPanels.push_back(new ContentBrowserPanel());
		m_WindowPanels.push_back(new LoggingOutputPanel());
		m_WindowPanels.push_back(new BuildGamePanel());

		for (EditorWindowPanel* layer : m_WindowPanels)
			layer->OnCreate();

		ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;

		ImGuiThemes::Dark();

		Rendering::RenderingSystem::Setup();
	}

	void EditorLayer::OnDestroy() {
		for (auto* layer : m_WindowPanels)
			delete layer;
		m_WindowPanels.clear();

		m_AssetManager.Shutdown();

		Rendering::RenderingSystem::Shutdown();
	}

	void EditorLayer::OnUpdate() {
		m_PanelKeyShortcutsPressed.resize(m_WindowPanels.size());
		for (int i = 0; i < m_WindowPanels.size(); i++) {
			bool pressed = m_WindowPanels[i]->Shortcut.size() > 0;
			for (auto keycode : m_WindowPanels[i]->Shortcut) {
				if (!Input::IsKeyDown(keycode))
					pressed = false;
			}
			m_PanelKeyShortcutsPressed[i] = pressed;
			m_WindowPanels[i]->OnUpdate();
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

	void EditorLayer::OnRender() {
		Rendering::Renderer::BeginScene();
		Rendering::RenderingSystem::RenderScene(*Engine::GetScene());
		Rendering::Renderer::EndScene();
	}

	void EditorLayer::OnUIRender() {
		if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DockingEnable) {
			static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

			ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
			const ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->WorkPos);
			ImGui::SetNextWindowSize(viewport->WorkSize);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

			if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
				window_flags |= ImGuiWindowFlags_NoBackground;

			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
			static bool opened = true;
			ImGui::Begin("DockSpace", &opened, window_flags);

			ImGui::PopStyleVar();
			ImGui::PopStyleVar(2);

			ImGuiIO& io = ImGui::GetIO();
			if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
				ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
				ImGui::DockSpace(dockspace_id, ImVec2(0, 0), dockspace_flags);
			}
		}

		ImGui::PushStyleColor(ImGuiCol_WindowBg, { 0.1f, 0.1f, 0.1f, 1.f });

		if (ImGui::BeginMenuBar()) {
			if (ImGui::BeginMenu("File")) {
				if (ImGui::MenuItem("Exit"))
					Pit::Engine::ForceShutdown();

				ImGui::EndMenu();
			}
			Array<bool> openWindows(m_WindowPanels.size());
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
				for (int i = 0; i < m_WindowPanels.size(); i++) {
					String shortcutStr;
					for (int j = 0; j < m_WindowPanels[i]->Shortcut.size(); j++) {
						shortcutStr += KeyCodeToString(m_WindowPanels[i]->Shortcut[j]);
						if (j + 1 != m_WindowPanels[i]->Shortcut.size()) shortcutStr += " + ";
					}
					if (ImGui::MenuItem(m_WindowPanels[i]->Name.c_str(), shortcutStr.c_str(), nullptr, m_WindowPanels[i]->Enabled))
						openWindows[i] = true;
				}
				ImGui::EndMenu();
			}
			for (int i = 0; i < m_WindowPanels.size(); i++) {
				if (openWindows[i] || m_PanelKeyShortcutsPressed[i])
					m_WindowPanels[i]->Opened = true;
			}

			if (ImGui::BeginMenu("Script")) {
				if (ImGui::MenuItem("Reload Assembly", "Ctrl + R"))
					Engine::Scripting()->ReloadAssembly();

				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}

		for (EditorWindowPanel* layer : m_WindowPanels)
			layer->OnGUI();

		ImGui::PopStyleColor();

		if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DockingEnable)
			ImGui::End();
	}

	void EditorLayer::OpenSceneFromFile() {
		String filepath = FileDialogs::OpenFile("PitEngine Scene (*.pitscene)\0*.pitscene\0");
		if (std::filesystem::exists(filepath)) {
			m_CurrentSceneFilepath = filepath;
			Engine::GetScene()->Clear();
			ECS::SceneSerializer::Deserialize(m_CurrentSceneFilepath, *Engine::GetScene());
		}
	}
	void EditorLayer::SaveSceneToFile() {
		String filepath = FileDialogs::SaveFile("PitEngine Scene (*.pitscene)\0*.pitscene\0");
		if (std::filesystem::exists(filepath)) {
			m_CurrentSceneFilepath = filepath;
			ECS::SceneSerializer::Serialize(m_CurrentSceneFilepath, *Engine::GetScene());
		}
	}
	void EditorLayer::SaveScene() {
		if (std::filesystem::exists(m_CurrentSceneFilepath)) {
			// Save scene
			ECS::SceneSerializer::Serialize(m_CurrentSceneFilepath, *Engine::GetScene());
		}
		else {
			String filepath = FileDialogs::SaveFile("PitEngine Scene (*.pitscene)\0*.pitscene\0");
			if (std::filesystem::exists(filepath)) {
				m_CurrentSceneFilepath = filepath;
				ECS::SceneSerializer::Serialize(m_CurrentSceneFilepath, *Engine::GetScene());
			}
		}
	}
}