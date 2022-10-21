#include "pch.hpp"
#include "EditorApplication.hpp"
#include "Panels/HierachyPanel.hpp"
#include "Panels/InspectorPanel.hpp"
#include "Panels/SceneViewportPanel.hpp"
#include "Panels/ProfilerPanel.hpp"
#include "Panels/ContentBrowserPanel.hpp"
#include "EditorDockspace.hpp"
#include <vulkan/vulkan.h>

using namespace Pit::Editor;

EditorApplication* EditorApplication::Instance = nullptr;

EditorApplication::EditorApplication(Engine& engine)
	: m_Engine(engine) {

	Instance = this;
}

EditorApplication::~EditorApplication() {
	Instance = nullptr;
}

void EditorApplication::Init() {
	m_AssetManager.Init();

	m_WindowPanels.push_back(new HierachyPanel());
	m_WindowPanels.push_back(new InspectorPanel());
	m_WindowPanels.push_back(new SceneViewportPanel());
	m_WindowPanels.push_back(new ProfilerPanel());
	m_WindowPanels.push_back(new ContentBrowserPanel());

	Engine::LayerManager()->SetCallbacks([&]() {EditorDockspace::OnBegin(MenubarCallback); }, [&]() {EditorDockspace::OnEnd(); });
	for (auto layer : m_WindowPanels)
		Engine::LayerManager()->PushLayer(layer);
}

void EditorApplication::Shutdown() {
	vkDeviceWaitIdle(Engine::Rendering()->Renderer->Device.device());

	m_AssetManager.Shutdown();
}

void EditorApplication::Update() {
	m_PanelKeyShortcutsPressed.resize(m_WindowPanels.size());
	for (int i = 0; i < m_WindowPanels.size(); i++) {
		bool pressed = m_WindowPanels[i]->Shortcut.size() > 0;
		for (auto keycode : m_WindowPanels[i]->Shortcut) {
			if (!Input::IsKeyDown(keycode))
				pressed = false;
		}
		m_PanelKeyShortcutsPressed[i] = pressed;
	}

	if (Input::IsKeyDown(Space)) {
		test1();
	}
}

void EditorApplication::MenubarCallback() {
	std::vector<bool> openWindows(EditorApplication::Instance->m_WindowPanels.size());
	if (ImGui::BeginMenu("Windows")) {
		for (int i = 0; i < EditorApplication::Instance->m_WindowPanels.size(); i++) {
			if (ImGui::MenuItem(EditorApplication::Instance->m_WindowPanels[i]->Name.c_str(), "wip", nullptr, EditorApplication::Instance->m_WindowPanels[i]->Enabled))
				openWindows[i] = true;
		}
		ImGui::EndMenu();
	}
	for (int i = 0; i < EditorApplication::Instance->m_WindowPanels.size(); i++) {
		if (openWindows[i] || EditorApplication::Instance->m_PanelKeyShortcutsPressed[i])
			EditorApplication::Instance->m_WindowPanels[i]->Opened = true;
	}
}