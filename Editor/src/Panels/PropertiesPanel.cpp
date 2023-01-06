#include "pch.hpp"
#include "Core/Engine.hpp"
#include "Scripting/ScriptingSubmodule.hpp"
#include "PropertiesPanel.hpp"
#include "HierachyPanel.hpp"
#include "UI/UIFonts.hpp"
#include "UI/UI.hpp"
#include "ECS/ECSSubmodule.hpp"
#include "ECS/ECSScene.hpp"
#include "ECS/ECSComponents.hpp"
#include <imgui/imgui.h>
#include <imgui/imgui_stdlib.h>
#pragma warning(push)
#pragma warning(disable: 4201)
#include <glm/gtc/type_ptr.hpp>
#pragma warning(pop)
#include <imgui/imgui_internal.h>

using namespace Pit;
using namespace UI;
using namespace Editor;

void PropertiesPanel::OnCreate() {
	Name = "Properties";
	Shortcut = Array<KeyCode>{ KeyCode::LeftControl, KeyCode::I };
}

void PropertiesPanel::OnDestroy() {

}

void PropertiesPanel::OnGui() {
	auto& ecsworld = Engine::ECS()->GetEcsWorld();
	if (ecsworld.GetRegistry().valid(HierachyPanel::s_SelectedEntity.GetID())) {
		_DrawComponents(HierachyPanel::s_SelectedEntity);
	}
}

template<typename T, typename UIFunction>
static void DrawComponent(const String& name, ECS::EntityHandle entity, UIFunction uifunction) {
	if (!entity.HasComponent<T>()) return;

	const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;

	auto& component = entity.GetComponent<T>();
	ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();

	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
	float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
	ImGui::Separator();
	bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), treeNodeFlags, name.c_str());
	ImGui::PopStyleVar(
	);
	ImGui::SameLine(contentRegionAvailable.x - lineHeight / 2);
	ImGui::PushFont(Fonts::Get(Fonts::ExtraBold));
	if (ImGui::Button("+", ImVec2{ lineHeight, lineHeight }))
		ImGui::OpenPopup("ComponentSettings");
	
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
		entity.RemoveComponent<T>();
}

template<typename T>
static void DisplayAddComponentEntry(const String& entryName) {
	if (!HierachyPanel::s_SelectedEntity.HasComponent<T>()) {
		if (ImGui::MenuItem(entryName.c_str())) {
			HierachyPanel::s_SelectedEntity.AddComponent<T>();
			ImGui::CloseCurrentPopup();
		}
	}
}

void PropertiesPanel::_DrawComponents(ECS::EntityHandle entity) {
	if (entity.HasComponent<ECS::NameComponent>()) {
		auto& nameComp = entity.GetComponent<ECS::NameComponent>();

		ImGui::InputText("Name", &nameComp.Name);
	}

	DrawComponent<ECS::TransformComponent>("Transform", entity, [](ECS::TransformComponent& transform) {
		UI::Vec3("Position", transform.Position);
		glm::vec3 rotation = glm::degrees(transform.Rotation);
		UI::Vec3("Rotation", rotation);
		transform.Rotation = glm::radians(rotation);
		UI::Vec3("Scale", transform.Scale, 1.0f);
	});

	DrawComponent<ECS::UUIDComponent>("UUID", entity, [](ECS::UUIDComponent& uuid) {
		ImGui::Text("UUID: %d", uuid.Id);
	});

	DrawComponent<ECS::CSharpComponent>("CSharpComponent", entity, [](ECS::CSharpComponent& component) {
		bool componentClassExists = Engine::Scripting()->ComponentClassExists(component.ClassName);

		if (!componentClassExists)
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.9f, 0.2f, 0.3f, 1.0f));

		ImGui::InputText("ComponentName", &component.ClassName);

		if (!componentClassExists)
			ImGui::PopStyleColor();
	});

	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Spacing();
	if (ImGui::Button("Add Component"))
		ImGui::OpenPopup("AddComponent");

	if (ImGui::BeginPopup("AddComponent")) {
		DisplayAddComponentEntry<ECS::TransformComponent>("Transform");
		DisplayAddComponentEntry<ECS::UUIDComponent>("UUID");
		DisplayAddComponentEntry<ECS::CSharpComponent>("CSharpComponent");
		ImGui::EndPopup();
	}
}