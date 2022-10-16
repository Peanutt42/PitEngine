#include "pch.hpp"
#include "SceneViewportPanel.hpp"
#include "Main/Engine.hpp"
#include <imgui/imgui.h>
#include <stb_image.h>
#include <vulkan/vulkan.h>

using namespace Pit::Editor;

void SceneViewportPanel::OnCreate() {
	Name = "Viewport";
	Shortcut = std::vector<KeyCode>{ LeftControl, T };
	m_SceneTexture = new Pit::Rendering::Texture("C:/Users/Peter/Pictures/Wallpaper/texture.jpeg");
}


void SceneViewportPanel::OnDestroy() {
	delete m_SceneTexture;
}

void SceneViewportPanel::OnGui() {
	auto region = ImGui::GetContentRegionAvail();
	ImGui::Image((ImTextureID)m_SceneTexture->GetDescriptorSet(),
				 ImVec2(region.x, region.y));
	if (ImGui::BeginDragDropTarget()) {
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(CONTENT_BROWSER_DRAG_DROP)) {
			const char* path = Cast<const char*>(payload->Data);
		}
		ImGui::EndDragDropTarget();
	}
}