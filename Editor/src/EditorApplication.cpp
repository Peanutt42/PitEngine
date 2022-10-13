#include "pch.hpp"
#include "EditorApplication.hpp"
#include "Panels/HierachyPanel.hpp"
#include "Panels/InspectorPanel.hpp"
#include "Panels/SceneViewportPanel.hpp"
#include "Panels/ProfilerPanel.hpp"
#include "Panels/ContentBrowserPanel.hpp"

using namespace Pit::Editor;


EditorApplication::EditorApplication(Engine& engine)
	: m_Engine(engine) {


}

EditorApplication::~EditorApplication() {

}

void EditorApplication::Init() {
	m_Panels.push_back(new HierachyPanel());
	m_Panels.push_back(new InspectorPanel());
	m_Panels.push_back(new SceneViewportPanel());
	m_Panels.push_back(new ProfilerPanel());
	m_Panels.push_back(new ContentBrowserPanel());

	for (auto layer : m_Panels)
		Engine::LayerManager()->PushLayer(layer);
	
	Engine::Instance->UIRenderer->SetMenubarCallback([&]() {
		std::vector<bool> openWindows(m_Panels.size());
		if (ImGui::BeginMenu("Windows"))  {
			for (int i = 0; i < m_Panels.size(); i++) {
				if (ImGui::MenuItem(m_Panels[i]->Name.c_str(), "wip", nullptr, m_Panels[i]->Enabled))
					openWindows[i] = true;
			}
			ImGui::EndMenu();
		}
		for (int i = 0; i < m_Panels.size(); i++) {
			if (openWindows[i] || m_PanelKeyShortcutsPressed[i])
				m_Panels[i]->Opened = true;
		}
	});
}

void EditorApplication::Shutdown() {

}

void EditorApplication::Update() {
	m_PanelKeyShortcutsPressed.resize(m_Panels.size());
	for (int i = 0; i < m_Panels.size(); i++) {
		bool pressed = m_Panels[i]->Shortcut.size() <= 0 ?
						false : true;
		for (auto keycode : m_Panels[i]->Shortcut) {
			if (!Input::IsKeyDown(keycode))
				pressed = false;
		}
		m_PanelKeyShortcutsPressed[i] = pressed;
	}
}