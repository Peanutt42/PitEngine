#include "pch.hpp"
#include "SceneViewportPanel.hpp"
#include "Core/Engine.hpp"
#include <imgui/imgui.h>
#include <stb_image.h>

using namespace Pit;
using namespace Editor;

void SceneViewportPanel::OnCreate() {
	Name = "Viewport";
	Shortcut = Array<KeyCode>{ LeftControl, Key_T };

	m_SceneTexture = new Pit::Rendering::Texture("C:/Users/Peter/Pictures/Wallpaper/texture.jpeg");
}


void SceneViewportPanel::OnDestroy() {
	delete m_SceneTexture;
}

void SceneViewportPanel::OnGui() {
	auto region = ImGui::GetContentRegionAvail();
	ImGui::Image((ImTextureID)Cast<uint64>(m_SceneTexture->GetRendererID()),
				 ImVec2(region.x, region.y));

	if (ImGui::BeginDragDropTarget()) {
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(CONTENT_BROWSER_DRAG_DROP)) {
			const char* path = Cast<const char*>(payload->Data);
		}
		ImGui::EndDragDropTarget();
	}
}