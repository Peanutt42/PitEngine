#include "pch.hpp"
#include "InspectorPanel.hpp"
#include "HierachyPanel.hpp"
#include "Engine/Main/Engine.hpp"
#include "Engine/ECS/ECSWorld.hpp"
#include "Engine/ECS/Commons/ECSEntityComponent.hpp"
#include "Engine/ECS/Commons/ECSTransformComponent.hpp"
#include "Engine/Rendering/UI/UIFonts.hpp"
#include <imgui/imgui.h>
#include <glm/gtc/type_ptr.hpp>
#include <imgui/imgui_internal.h>

using namespace Pit::Editor;

void InspectorPanel::OnAttach() {
	Name = "Inspector";
	Shortcut = std::vector<KeyCode>{ LeftControl, I };
}

void InspectorPanel::OnDetach() {

}

void InspectorPanel::OnGUI() {
	ImGui::Begin(Name.c_str(), &Opened);

	auto* ecsworld = Engine::Instance->ECSSubsystem->GetEcsWorld();
	if (ecsworld->GetRegistry().valid(HierachyPanel::s_SelectedEntity)) {
		_DrawComponents(ecsworld, HierachyPanel::s_SelectedEntity);
	}

	ImGui::End();
}

static void DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f) {
	ImFont* extraBoldFont = Pit::Rendering::UI::GetFont(Pit::Rendering::UI::ExtraBold);

	ImGui::PushID(label.c_str());

	ImGui::Columns(2);
	ImGui::SetColumnWidth(0, columnWidth);
	ImGui::Text(label.c_str());
	ImGui::NextColumn();

	ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

	float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
	ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
	ImGui::PushFont(extraBoldFont);
	if (ImGui::Button("X", buttonSize))
		values.x = resetValue;
	ImGui::PopFont();
	ImGui::PopStyleColor(3);

	ImGui::SameLine();
	ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
	ImGui::PopItemWidth();
	ImGui::SameLine();

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
	ImGui::PushFont(extraBoldFont);
	if (ImGui::Button("Y", buttonSize))
		values.y = resetValue;
	ImGui::PopFont();
	ImGui::PopStyleColor(3);

	ImGui::SameLine();
	ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
	ImGui::PopItemWidth();
	ImGui::SameLine();

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
	ImGui::PushFont(extraBoldFont);
	if (ImGui::Button("Z", buttonSize))
		values.z = resetValue;
	ImGui::PopFont();
	ImGui::PopStyleColor(3);

	ImGui::SameLine();
	ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
	ImGui::PopItemWidth();

	ImGui::PopStyleVar();

	ImGui::Columns(1);

	ImGui::PopID();
}

template<typename T, typename UIFunction>
static void DrawComponent(const std::string& name, Pit::ECS::World* world, entt::entity entity, UIFunction uifunction) {
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
	ImGui::PushFont(Pit::Rendering::UI::GetFont(Pit::Rendering::UI::ExtraBold));
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
static void DisplayAddComponentEntry(const std::string& entryName) {
	auto* ecsworld = Pit::Engine::Instance->ECSSubsystem->GetEcsWorld();
	if (!ecsworld->HasComponent<T>(HierachyPanel::s_SelectedEntity)) {
		if (ImGui::MenuItem(entryName.c_str())) {
			ecsworld->AddComponent<T>(HierachyPanel::s_SelectedEntity);
			ImGui::CloseCurrentPopup();
		}
	}
}

void InspectorPanel::_DrawComponents(ECS::World* world, entt::entity entity) {
	if (world->HasComponent<ECS::EntityComponent>(entity)) {
		auto& entityComp = world->GetComponent<ECS::EntityComponent>(entity);
		auto& name = entityComp.Name;

		char buffer[256];
		memset(buffer, 0, sizeof(buffer));
		strcpy_s(buffer, name.c_str());
		if (ImGui::InputText("Name", buffer, sizeof(buffer)))
			name = std::string(buffer);
	}

	DrawComponent<ECS::TransformComponent>("Transform", world, entity, [](ECS::TransformComponent& component) {
		DrawVec3Control("Translation", component.position);
		glm::vec3 rotation = glm::degrees(component.rotation);
		DrawVec3Control("Rotation", rotation);
		component.rotation = glm::radians(rotation);
		DrawVec3Control("Scale", component.scale, 1.0f);
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
		// DisplayAddComponentEntry<CameraComponent>("Camera");// Example
		ImGui::EndPopup();
	}
}