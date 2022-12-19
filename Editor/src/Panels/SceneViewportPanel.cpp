#include "pch.hpp"
#include "SceneViewportPanel.hpp"
#include "Core/Engine.hpp"
#include "Rendering/RenderingSubmodule.hpp"
#include "Rendering/SpectatorCamera.hpp"
#include <imgui/imgui.h>
#include <stb_image.h>

using namespace Pit;
using namespace Editor;

#define EDITOR_KEYBINDING_CAM_MOVE_FORWARD "_Editor_Cam_MoveForward"
#define EDITOR_KEYBINDING_CAM_MOVE_BACKWARD "_Editor_Cam_MoveBackward"
#define EDITOR_KEYBINDING_CAM_MOVE_LEFT "_Editor_Cam_MoveLeft"
#define EDITOR_KEYBINDING_CAM_MOVE_RIGHT "_Editor_Cam_MoveRight"
#define EDITOR_KEYBINDING_CAM_MOVE_UP "_Editor_Cam_MoveUp"
#define EDITOR_KEYBINDING_CAM_MOVE_DOWN "_Editor_Cam_MoveDown"
#define EDITOR_KEYBINDING_CAM_MOVE_FASTER "_Editor_Cam_MoveFaster"
#define EDITOR_KEYBINDING_CAM_MOVE_SLOWER "_Editor_Cam_MoveSlower"

void SceneViewportPanel::OnCreate() {
	Name = "Viewport";
	Shortcut = Array<KeyCode>{ KeyCode::LeftControl, KeyCode::T };

	if (!Engine::GetSettings().Headless)
		m_SceneTexture = new Pit::Rendering::Texture("C:/Users/Peter/Pictures/Wallpaper/texture.jpeg");

	Input::AddBinding(EDITOR_KEYBINDING_CAM_MOVE_FORWARD, KeyCode::W);
	Input::AddBinding(EDITOR_KEYBINDING_CAM_MOVE_BACKWARD, KeyCode::S);
	Input::AddBinding(EDITOR_KEYBINDING_CAM_MOVE_LEFT , KeyCode::A);
	Input::AddBinding(EDITOR_KEYBINDING_CAM_MOVE_RIGHT, KeyCode::D);
	Input::AddBinding(EDITOR_KEYBINDING_CAM_MOVE_UP, KeyCode::E);
	Input::AddBinding(EDITOR_KEYBINDING_CAM_MOVE_DOWN, KeyCode::Q);
	Input::AddBinding(EDITOR_KEYBINDING_CAM_MOVE_FASTER, KeyCode::LeftShift);
	Input::AddBinding(EDITOR_KEYBINDING_CAM_MOVE_SLOWER, KeyCode::LeftControl);
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

	if (Input::IsMouseButtonDown(MouseButton::Button1)) {
		SpectatorCamera::Update(Engine::Rendering()->Camera,
								Input::GetBinding(EDITOR_KEYBINDING_CAM_MOVE_FORWARD),
								Input::GetBinding(EDITOR_KEYBINDING_CAM_MOVE_BACKWARD),
								Input::GetBinding(EDITOR_KEYBINDING_CAM_MOVE_LEFT),
								Input::GetBinding(EDITOR_KEYBINDING_CAM_MOVE_RIGHT),
								Input::GetBinding(EDITOR_KEYBINDING_CAM_MOVE_UP),
								Input::GetBinding(EDITOR_KEYBINDING_CAM_MOVE_DOWN),
								Input::GetBinding(EDITOR_KEYBINDING_CAM_MOVE_FASTER),
								Input::GetBinding(EDITOR_KEYBINDING_CAM_MOVE_SLOWER));

	
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
			[[maybe_unused]] char* path = Cast<char*>(payload->Data);
			PIT_ENGINE_INFO(Editor, "Scene Viewport DragDrop Accepted: path: {}", path);
		}
		ImGui::EndDragDropTarget();
	}
}