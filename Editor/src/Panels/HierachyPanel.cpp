#include "pch.hpp"
#include "Core/Engine.hpp"
#include "HierachyPanel.hpp"
#include "ECS/ECSSubmodule.hpp"
#include "ECS/ECSComponents.hpp"
#include <imgui/imgui.h>

using namespace Pit::Editor;

entt::entity HierachyPanel::s_SelectedEntity = entt::null;

void HierachyPanel::OnCreate() {
	Name = "Hierachy";
	Shortcut = Array<KeyCode>{ LeftControl + Key_H };

	auto& ecsworld = Engine::ECS()->GetEcsWorld();
	for (int i = 0; i < 15; i++) {
		auto e = ecsworld.CreateEntity();
		e.GetComponent<ECS::NameComponent>().Name = "Entity" + std::to_string((uint32)e.GetID());
	}
}

void HierachyPanel::OnDestroy() {
}

void HierachyPanel::OnGui() {
	auto& ecsworld = Engine::ECS()->GetEcsWorld();
	ecsworld.Each([&](entt::entity e) {
		if (ecsworld.GetRegistry().valid(e))
			_DrawEntityNode(e);
	});

	if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
		s_SelectedEntity = entt::null;

	// Right-click on blank space
	if (ImGui::BeginPopupContextWindow("EntityCreateMenu", ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems | ImGuiPopupFlags_NoOpenOverExistingPopup)) {
		if (ImGui::MenuItem("Create Entity"))
			ecsworld.CreateEntity();

		ImGui::EndPopup();
	}
}

void HierachyPanel::_DrawEntityNode(entt::entity entity) {
	auto& ecsworld = Engine::ECS()->GetEcsWorld();
	auto& name = ecsworld.GetComponent<ECS::NameComponent>(entity).Name;

	ImGuiTreeNodeFlags flags = ((s_SelectedEntity == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
	flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

	bool opened = ImGui::TreeNodeEx((void*)(uint64)(uint32)entity, flags, name.c_str());

	if (ImGui::IsItemClicked())
		s_SelectedEntity = entity;

	bool deleteEntity = false;
	if (ImGui::BeginPopupContextItem()) {
		deleteEntity = ImGui::MenuItem("Delete Entity");
		ImGui::EndPopup();
	}
	if (opened)
		ImGui::TreePop();

	if (deleteEntity) {
		ecsworld.DestroyEntity(entity);
		s_SelectedEntity = entt::null;
	}
}