// Copied (and modified) from TheCherno/Hazel https://github.com/TheCherno/Hazel/blob/master/Hazelnut/src/Panels/SceneHierarchyPanel.cpp, see SceneHierachyPanel

#include "pch.hpp"
#include "Core/Engine.hpp"
#include "HierachyPanel.hpp"
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
	Engine::GetScene()->Each([this](entt::entity e) {
		if (Engine::GetScene()->GetRegistry().valid(e))
			_DrawEntityNode(e);
	});

	if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
		s_SelectedEntity = ECS::EntityHandle(Engine::GetScene(), entt::null);

	// Right-click on blank space
	if (ImGui::BeginPopupContextWindow("EntityCreateMenu", ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems | ImGuiPopupFlags_NoOpenOverExistingPopup)) {
		if (ImGui::MenuItem("Create Entity")) {
			ECS::EntityHandle entity = Engine::GetScene()->CreateEntity();
			s_SelectedEntity = entity;
		}

		ImGui::EndPopup();
	}
}

void HierachyPanel::_DrawEntityNode(entt::entity entity) {
	auto& ecsworld = *Engine::GetScene();
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