#include "pch.hpp"
#include "SceneViewportPanel.hpp"
#include "Core/Engine.hpp"
#include "Rendering/RenderingSubmodule.hpp"
#include <imgui/imgui.h>
#include <stb_image.h>

using namespace Pit;
using namespace Editor;

#define EDITOR_KEYBINDING_CAM_MOVE_FORWARD "_Editor_Cam_MoveForward"
#define EDITOR_KEYBINDING_CAM_MOVE_BACKWARD "_Editor_Cam_MoveBackward"
#define EDITOR_KEYBINDING_CAM_MOVE_LEFT "_Editor_Cam_MoveLeft"
#define EDITOR_KEYBINDING_CAM_MOVE_RIGHT "_Editor_Cam_MoveRight"

void SceneViewportPanel::OnCreate() {
	Name = "Viewport";
	Shortcut = Array<KeyCode>{ KeyCode::LeftControl, KeyCode::T };

	if (!Engine::GetSettings().Headless)
		m_SceneTexture = new Pit::Rendering::Texture("C:/Users/Peter/Pictures/Wallpaper/texture.jpeg");

	Input::AddBinding(EDITOR_KEYBINDING_CAM_MOVE_FORWARD, KeyCode::W);
	Input::AddBinding(EDITOR_KEYBINDING_CAM_MOVE_BACKWARD, KeyCode::S);
	Input::AddBinding(EDITOR_KEYBINDING_CAM_MOVE_LEFT , KeyCode::A);
	Input::AddBinding(EDITOR_KEYBINDING_CAM_MOVE_RIGHT, KeyCode::D);
}


void SceneViewportPanel::OnDestroy() {
	delete m_SceneTexture;
}

void SceneViewportPanel::OnGui() {
	auto region = ImGui::GetContentRegionAvail();
	auto panelSize = ImGui::GetWindowSize();
	auto panelPos = ImGui::GetWindowPos();
	ImGui::Image((ImTextureID)Cast<uint64>(Engine::Rendering()->Renderer->GetScreenTexColorBuffer()),
				 ImVec2(region.x, region.y), ImVec2(0, 1), ImVec2(1, 0));

	Rendering::Camera* cam = &Engine::Rendering()->Renderer->m_Camera;
	if (Input::IsMouseButtonDown(MouseButton::Button1)) {
		cam->ProcessMouseMovement(Input::GetMouseDelta().x, -Input::GetMouseDelta().y);

		if (Input::IsKeyDown(Input::GetBinding(EDITOR_KEYBINDING_CAM_MOVE_FORWARD))) cam->ProcessKeyboard({ 0, 1, 0 });
		if (Input::IsKeyDown(Input::GetBinding(EDITOR_KEYBINDING_CAM_MOVE_BACKWARD))) cam->ProcessKeyboard({ 0, -1, 0 });
		if (Input::IsKeyDown(Input::GetBinding(EDITOR_KEYBINDING_CAM_MOVE_LEFT))) cam->ProcessKeyboard({ 1, 0, 0 });
		if (Input::IsKeyDown(Input::GetBinding(EDITOR_KEYBINDING_CAM_MOVE_RIGHT))) cam->ProcessKeyboard({ -1, 0, 0 });
	
		glm::vec2 windowPos;
		Engine::Rendering()->Window->GetPosition(windowPos.x, windowPos.y);

		// Left Border
		if (Input::GetMousePos().x + windowPos.x <= panelPos.x) Input::SetMousePos({ panelSize.x + panelPos.x - windowPos.x, Input::GetMousePos().y});
		// Right Border
		if (Input::GetMousePos().x + windowPos.x > panelPos.x + panelSize.x) Input::SetMousePos({ panelPos.x - windowPos.x + 1, Input::GetMousePos().y });
		// Top Border
		if (Input::GetMousePos().y + windowPos.y - panelPos.y <= panelPos.y) Input::SetMousePos({Input::GetMousePos().x, panelPos.y + panelSize.y - windowPos.y - 1});
		// Bottom Border
		if (Input::GetMousePos().y + windowPos.y - panelPos.y >= panelSize.y) Input::SetMousePos({ Input::GetMousePos().x, panelPos.y - windowPos.y + 50 });
	}	
	

	if (ImGui::BeginDragDropTarget()) {
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(CONTENT_BROWSER_DRAG_DROP)) {
			[[maybe_unused]] const char* path = Cast<const char*>(payload->Data);
			PIT_ENGINE_INFO(Editor, "Scene Viewport DragDrop Accepted: path: {}", path);
		}
		ImGui::EndDragDropTarget();
	}
}