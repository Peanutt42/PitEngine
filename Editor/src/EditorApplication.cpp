#include "EditorApplication.hpp"
#include "Panels/EntityListPanel.hpp"

using namespace Pit::Editor;

EditorApplication::EditorApplication(Engine& engine)
	: m_Engine(engine) {


}

EditorApplication::~EditorApplication() {

}

void EditorApplication::Init() {
	m_Engine.GetUILayerManager()->PushLayer<EntityListPanel>();
}

void EditorApplication::Shutdown() {

}

void EditorApplication::Update() {

}