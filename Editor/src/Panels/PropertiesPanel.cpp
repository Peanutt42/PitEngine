#include "pch.hpp"
#include "Core/Engine.hpp"
#include "Scripting/ScriptingSubmodule.hpp"
#include "PropertiesPanel.hpp"
#include "HierachyPanel.hpp"
#include "UI/UIFonts.hpp"
#include "UI/UI.hpp"
#include "ECS/ECSSubmodule.hpp"
#include "ECS/ECSWorld.hpp"
#include "ECS/ECSComponents.hpp"
#include <imgui/imgui.h>
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
		auto& entityComp = entity.GetComponent<ECS::NameComponent>();
		auto& name = entityComp.Name;

		static char buffer[256];
		memset(buffer, 0, sizeof(buffer));
		strcpy_s(buffer, name.c_str());
		if (ImGui::InputText("Name", buffer, sizeof(buffer)))
			name = String(buffer);
	}

	DrawComponent<ECS::TransformComponent>("Transform", entity, [](ECS::TransformComponent& transform) {
		UI::Vec3("Position", transform.position);
		glm::vec3 rotation = glm::degrees(transform.rotation);
		UI::Vec3("Rotation", rotation);
		transform.rotation = glm::radians(rotation);
		UI::Vec3("Scale", transform.scale, 1.0f);
	});

	DrawComponent<ECS::UUIDComponent>("UUID", entity, [](ECS::UUIDComponent& uuid) {
		ImGui::Text("UUID: %d", uuid.Id);
	});

	DrawComponent<ECS::ScriptComponent>("Script", entity, [](ECS::ScriptComponent& script) {
		bool scriptClassExists = Engine::Scripting()->EntityClassExists(script.Name);
		
		if (!scriptClassExists) ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.9f, 0.2f, 0.3f, 1.0f));

		static char buffer[64];
		strcpy_s(buffer, script.Name.c_str());
		if (ImGui::InputText("Class", buffer, 64))
			script.Name = buffer;

		if (!scriptClassExists) ImGui::PopStyleColor();
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
		//DisplayAddComponentEntry<ECS::CameraComponent>("Camera");// Example
		DisplayAddComponentEntry<ECS::TransformComponent>("Transform");
		DisplayAddComponentEntry<ECS::UUIDComponent>("UUID");
		DisplayAddComponentEntry<ECS::ScriptComponent>("Script");
		ImGui::EndPopup();
	}
}