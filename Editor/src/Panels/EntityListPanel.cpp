#include "EntityListPanel.hpp"
#include <imgui/imgui.h>

using namespace Pit::Editor;

void EntityListPanel::OnAttach() {
}

void EntityListPanel::OnDetach() {
}

void EntityListPanel::OnGUI() {
	ImGui::Begin("EntityList");

	ImGui::Text("Hello World!");

	ImGui::End();
	
	
	ImGui::Begin("EntityList2");

	ImGui::Text("Hello World!");

	ImGui::End();
}