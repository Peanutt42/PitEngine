#include "pch.hpp"
#include "SceneViewportPanel.hpp"
#include "Core/Engine.hpp"
#include "Rendering/RenderingSubmodule.hpp"
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
	ImGui::Image((ImTextureID)Cast<uint64>(Engine::Rendering()->Renderer->GetScreenTexColorBuffer()),
				 ImVec2(region.x, region.y), ImVec2(0, 1), ImVec2(1, 0));

	if (ImGui::BeginDragDropTarget()) {
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(CONTENT_BROWSER_DRAG_DROP)) {
			const char* path = Cast<const char*>(payload->Data);
		}
		ImGui::EndDragDropTarget();
	}
}