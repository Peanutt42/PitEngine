#include "HierachyPanel.hpp"
#include <imgui/imgui.h>
#include "Engine/Main/Engine.hpp"
#include "Engine/ECS/Commons/ECSEntityComponent.hpp"

using namespace Pit::Editor;

entt::entity HierachyPanel::s_SelectedEntity = entt::null;

void HierachyPanel::OnAttach() {
	auto* ecsworld = Engine::Instance->ECSSubsystem->GetEcsWorld();
	for (int i = 0; i < 15; i++) {
		auto e = ecsworld->CreateEntity();
		ecsworld->GetComponent<ECS::EntityComponent>(e).Name = "Entity" + std::to_string((uint32_t)e);
	}
}

void HierachyPanel::OnDetach() {
}

void HierachyPanel::OnGUI() {
	ImGui::Begin("EntityList");

	auto* ecsworld = Engine::Instance->ECSSubsystem->GetEcsWorld();
	ecsworld->Each([&](entt::entity e) {
		if (ecsworld->GetRegistry().valid(e))
			_DrawEntityNode(e);
	});

	if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
		s_SelectedEntity = entt::null;

	// Right-click on blank space
	if (ImGui::BeginPopupContextWindow("EntityCreateMenu", ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems | ImGuiPopupFlags_NoOpenOverExistingPopup)) {
		if (ImGui::MenuItem("Create Entity"))
			ecsworld->CreateEntity();

		ImGui::EndPopup();
	}

	ImGui::End();
}

void HierachyPanel::_DrawEntityNode(entt::entity entity) {
	auto* ecsworld = Engine::Instance->ECSSubsystem->GetEcsWorld();
	auto& name = ecsworld->GetComponent<ECS::EntityComponent>(entity).Name;

	ImGuiTreeNodeFlags flags = ((s_SelectedEntity == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
	flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

	bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, name.c_str());

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
		ecsworld->DestroyEntity(entity);
		s_SelectedEntity = entt::null;
	}
}