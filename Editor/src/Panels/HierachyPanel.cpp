#include "pch.hpp"
#include "Core/Engine.hpp"
#include "HierachyPanel.hpp"
#include "ECS/ECSSubmodule.hpp"
#include "ECS/ECSComponents.hpp"
#include "ECS/ECSEntityHandle.hpp"
#include <imgui/imgui.h>

using namespace Pit::Editor;

Pit::ECS::EntityHandle HierachyPanel::s_SelectedEntity = ECS::EntityHandle(nullptr, entt::null);

void HierachyPanel::OnCreate() {
	Name = "Hierachy";
	Shortcut = Array<KeyCode>{ KeyCode::LeftControl, KeyCode::H };
}

void HierachyPanel::OnDestroy() {
}

void HierachyPanel::OnGui() {
	Engine::ECS()->GetEcsWorld().Each([this](entt::entity e) {
		if (Engine::ECS()->GetEcsWorld().GetRegistry().valid(e))
			_DrawEntityNode(e);
	});

	if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
		s_SelectedEntity = ECS::EntityHandle(&Engine::ECS()->GetEcsWorld(), entt::null);

	// Right-click on blank space
	if (ImGui::BeginPopupContextWindow("EntityCreateMenu", ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems | ImGuiPopupFlags_NoOpenOverExistingPopup)) {
		if (ImGui::MenuItem("Create Entity"))
			Engine::ECS()->GetEcsWorld().CreateEntity();

		ImGui::EndPopup();
	}
}

void HierachyPanel::_DrawEntityNode(entt::entity entity) {
	auto& ecsworld = Engine::ECS()->GetEcsWorld();
	auto& name = ecsworld.GetComponent<ECS::NameComponent>(entity).Name;

	ImGuiTreeNodeFlags flags = ((s_SelectedEntity.GetID() == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
	flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

	bool opened = ImGui::TreeNodeEx((void*)(uint64)(uint32)entity, flags, name.c_str());

	if (ImGui::IsItemClicked())
		s_SelectedEntity = ECS::EntityHandle(&ecsworld, entity);

	bool deleteEntity = false;
	if (ImGui::BeginPopupContextItem()) {
		deleteEntity = ImGui::MenuItem("Delete Entity");
		ImGui::EndPopup();
	}
	if (opened)
		ImGui::TreePop();

	if (deleteEntity) {
		ecsworld.DestroyEntity(entity);
		s_SelectedEntity = ECS::EntityHandle(nullptr, entt::null);
	}
}