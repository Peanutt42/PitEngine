#include "pch.hpp"
#include "EditorApplication.hpp"
#include "Panels/HierachyPanel.hpp"
#include "Panels/InspectorPanel.hpp"
#include "Panels/SceneViewportPanel.hpp"

using namespace Pit::Editor;

EditorApplication::EditorApplication(Engine& engine)
	: m_Engine(engine) {


}

EditorApplication::~EditorApplication() {

}

void EditorApplication::Init() {
	m_Engine.LayerManager()->PushLayer<HierachyPanel>();
	m_Engine.LayerManager()->PushLayer<InspectorPanel>();
	m_Engine.LayerManager()->PushLayer<SceneViewportPanel>();
}

void EditorApplication::Shutdown() {

}

void EditorApplication::Update() {

}