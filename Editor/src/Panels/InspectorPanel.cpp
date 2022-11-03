#include "pch.hpp"
#include "InspectorPanel.hpp"
#include "HierachyPanel.hpp"
#include "Core/Engine.hpp"
#include "UI/UIFonts.hpp"
#include "ECS/ECSWorld.hpp"
#include "ECS/ECSComponents.hpp"
#include <imgui/imgui.h>
#include <glm/gtc/type_ptr.hpp>
#include <imgui/imgui_internal.h>

using namespace Pit;
using namespace UI;
using namespace Editor;

void InspectorPanel::OnCreate() {
	Name = "Inspector";
	Shortcut = Array<KeyCode>{ LeftControl, Key_I };
}

void InspectorPanel::OnDestroy() {

}

void InspectorPanel::OnGui() {
	auto& ecsworld = Engine::ECS()->GetEcsWorld();
	if (ecsworld.GetRegistry().valid(HierachyPanel::s_SelectedEntity)) {
		_DrawComponents(&ecsworld, HierachyPanel::s_SelectedEntity);
	}
}

template<typename T, typename UIFunction>
static void DrawComponent(const String& name, Pit::ECS::World* world, entt::entity entity, UIFunction uifunction) {
	if (!world->HasComponent<T>(entity)) return;

	const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;

	auto& component = world->GetComponent<T>(entity);
	ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();

	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
	float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
	ImGui::Separator();
	bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), treeNodeFlags, name.c_str());
	ImGui::PopStyleVar(
	);
	ImGui::SameLine(contentRegionAvailable.x - lineHeight * 0.5f);
	ImGui::PushFont(Fonts::Get(Fonts::ExtraBold));
	if (ImGui::Button("+", ImVec2{ lineHeight, lineHeight })) {
		ImGui::OpenPopup("ComponentSettings");
	}
	ImGui::PopFont();

	bool removeComponent = false;
	if (ImGui::BeginPopup("ComponentSettings")) {
		if (ImGui::MenuItem("Remove component"))
			removeComponent = true;

		ImGui::EndPopup();
	}

	if (open) {
		uifunction(component);
		ImGui::TreePop();
	}

	if (removeComponent)
		world->RemoveComponent<T>(entity);
}

template<typename T>
static void DisplayAddComponentEntry(const String& entryName) {
	auto& ecsworld = Pit::Engine::ECS()->GetEcsWorld();
	if (!ecsworld.HasComponent<T>(HierachyPanel::s_SelectedEntity)) {
		if (ImGui::MenuItem(entryName.c_str())) {
			ecsworld.AddComponent<T>(HierachyPanel::s_SelectedEntity);
			ImGui::CloseCurrentPopup();
		}
	}
}

void InspectorPanel::_DrawComponents(ECS::World* world, entt::entity entity) {
	if (world->HasComponent<ECS::NameComponent>(entity)) {
		auto& entityComp = world->GetComponent<ECS::NameComponent>(entity);
		auto& name = entityComp.Name;

		char buffer[256];
		memset(buffer, 0, sizeof(buffer));
		strcpy_s(buffer, name.c_str());
		if (ImGui::InputText("Name", buffer, sizeof(buffer)))
			name = String(buffer);
	}

	/*DrawComponent<ECS::TransformComponent>("Transform", world, entity, [](ECS::TransformComponent& component) {
		UI::Vec3("Translation", component.position);
		glm::vec3 rotation = glm::degrees(component.rotation);
		UI::Vec3("Rotation", rotation);
		component.rotation = glm::radians(rotation);
		UI::Vec3("Scale", component.scale, 1.0f);
	});*/

	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Spacing();
	if (ImGui::Button("Add Component"))
		ImGui::OpenPopup("AddComponent");

	if (ImGui::BeginPopup("AddComponent")) {
		// DisplayAddComponentEntry<CameraComponent>("Camera");// Example
		ImGui::EndPopup();
	}
}