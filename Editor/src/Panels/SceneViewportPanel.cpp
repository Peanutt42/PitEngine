#include "pch.hpp"
#include "SceneViewportPanel.hpp"
#include "Core/Engine.hpp"
#include "Rendering/RenderingSubmodule.hpp"
#include "Rendering/SpectatorCamera.hpp"
#include <imgui/imgui.h>

using namespace Pit;
using namespace Editor;

#define EDITOR_KEYBINDING_CAM_MOVE "_Editor_Cam_Move"
#define EDITOR_KEYBINDING_CAM_LOOK "_Editor_Cam_Look"
#define EDITOR_KEYBINDING_CAM_MOVE_UP "_Editor_Cam_MoveUp"
#define EDITOR_KEYBINDING_CAM_MOVE_DOWN "_Editor_Cam_MoveDown"
#define EDITOR_KEYBINDING_CAM_MOVE_FASTER "_Editor_Cam_MoveFaster"
#define EDITOR_KEYBINDING_CAM_MOVE_SLOWER "_Editor_Cam_MoveSlower"

void SceneViewportPanel::OnCreate() {
	Name = "Viewport";
	Shortcut = Array<KeyCode>{ KeyCode::LeftControl, KeyCode::T };

	Input::AddAxisBinding(EDITOR_KEYBINDING_CAM_MOVE, ControllerAxis::Left);
	Input::SetAxisBinding(EDITOR_KEYBINDING_CAM_MOVE, KeyAxis(KeyCode::W, KeyCode::S, KeyCode::A, KeyCode::D));
	Input::AddAxisBinding(EDITOR_KEYBINDING_CAM_LOOK, ControllerAxis::Right);
	Input::SetMouseAxisBinding(EDITOR_KEYBINDING_CAM_LOOK);
	Input::AddBinding(EDITOR_KEYBINDING_CAM_MOVE_UP, KeyCode::E);
	Input::AddBinding(EDITOR_KEYBINDING_CAM_MOVE_DOWN, KeyCode::Q);
	Input::AddBinding(EDITOR_KEYBINDING_CAM_MOVE_FASTER, KeyCode::LeftShift);
	Input::AddBinding(EDITOR_KEYBINDING_CAM_MOVE_SLOWER, KeyCode::LeftControl);
	if (Input::GetControllerButtonBinding(EDITOR_KEYBINDING_CAM_MOVE_UP) == ControllerButton::None)
		Input::SetBinding(EDITOR_KEYBINDING_CAM_MOVE_UP, ControllerButton::Down);
	if (Input::GetControllerButtonBinding(EDITOR_KEYBINDING_CAM_MOVE_DOWN) == ControllerButton::None)
		Input::SetBinding(EDITOR_KEYBINDING_CAM_MOVE_DOWN, ControllerButton::Right);
	if (Input::GetControllerButtonBinding(EDITOR_KEYBINDING_CAM_MOVE_FASTER) == ControllerButton::None)
		Input::SetBinding(EDITOR_KEYBINDING_CAM_MOVE_FASTER, ControllerButton::LeftStickPress);
	if (Input::GetControllerButtonBinding(EDITOR_KEYBINDING_CAM_MOVE_SLOWER) == ControllerButton::None)
		Input::SetBinding(EDITOR_KEYBINDING_CAM_MOVE_SLOWER, ControllerButton::RightStickPress);
}


void SceneViewportPanel::OnDestroy() {}

void SceneViewportPanel::OnGui() {
	auto region = ImGui::GetContentRegionAvail();
	auto panelSize = ImGui::GetWindowSize();
	auto panelPos = ImGui::GetWindowPos();
	ImGui::Image((ImTextureID)Cast<uint64>(Engine::Rendering()->Renderer->GetScreenTexColorBuffer()),
				 ImVec2(region.x, region.y), ImVec2(0, 1), ImVec2(1, 0));

	if (Input::IsKeyboardAndMouseInUse() && Input::IsMouseButtonDown(MouseButton::Right) ||
		Input::IsControllerInUse()) {
		glm::vec3 moveDir{ 0,0,0 };
		glm::vec2 moveInput = Input::GetAxisBinding(EDITOR_KEYBINDING_CAM_MOVE);
		moveDir.x = -moveInput.x;
		moveDir.z = moveInput.y;
		if (Input::IsBindingDown(EDITOR_KEYBINDING_CAM_MOVE_UP))   moveDir.y++;
		if (Input::IsBindingDown(EDITOR_KEYBINDING_CAM_MOVE_DOWN)) moveDir.y--;
		glm::vec2 lookDir = Input::GetAxisBinding(EDITOR_KEYBINDING_CAM_LOOK);
		lookDir.y = -lookDir.y;
		SpectatorCamera::Update(Engine::Rendering()->Camera,
								moveDir,
								lookDir,
								Input::IsBindingDown(EDITOR_KEYBINDING_CAM_MOVE_FASTER),
								Input::IsBindingDown(EDITOR_KEYBINDING_CAM_MOVE_SLOWER));

		if (Input::IsKeyboardAndMouseInUse()) {
			glm::vec2 windowPos;
			Engine::Rendering()->Window->GetPosition(windowPos.x, windowPos.y);

			// Left Border
			if (Input::GetMousePos().x + windowPos.x <= panelPos.x) Input::SetMousePos({ panelSize.x + panelPos.x - windowPos.x, Input::GetMousePos().y });
			// Right Border
			if (Input::GetMousePos().x + windowPos.x > panelPos.x + panelSize.x) Input::SetMousePos({ panelPos.x - windowPos.x + 1, Input::GetMousePos().y });
			// Top Border
			if (Input::GetMousePos().y + windowPos.y - panelPos.y <= panelPos.y) Input::SetMousePos({ Input::GetMousePos().x, panelPos.y + panelSize.y - windowPos.y - 1 });
			// Bottom Border
			if (Input::GetMousePos().y + windowPos.y - panelPos.y >= panelSize.y) Input::SetMousePos({ Input::GetMousePos().x, panelPos.y - windowPos.y + 50 });
		}
	}
	

	if (ImGui::BeginDragDropTarget()) {
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(CONTENT_BROWSER_DRAG_DROP)) {
			[[maybe_unused]] char* path = Cast<char*>(payload->Data);
			PIT_ENGINE_INFO(Editor, "Scene Viewport DragDrop Accepted: path: {}", path);
		}
		ImGui::EndDragDropTarget();
	}
}