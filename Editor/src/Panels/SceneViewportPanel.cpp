#include "pch.hpp"
#include "SceneViewportPanel.hpp"
#include "HierachyPanel.hpp"
#include "Core/Engine.hpp"
#include "Rendering/RenderingSubmodule.hpp"
#include "ECS/ECSSubmodule.hpp"
#include "Rendering/Texture.hpp"
#include "Rendering/SpectatorCamera.hpp"
#include "UI/UI.hpp"
#include <imgui/imgui.h>
#include <ImGuizmo.h>
#include "Math/GraphicsMath.hpp"


namespace Pit::Editor {

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

	static void UIToolbar();

	void SceneViewportPanel::OnGui() {
		m_ViewportSize = ImGui::GetContentRegionAvail();
		ImVec2 panelSize = ImGui::GetWindowSize();
		ImVec2 panelPos = ImGui::GetWindowPos();
		ImGui::Image((ImTextureID)Cast<uint64>(Engine::Rendering()->Renderer->GetScreenFramebuffer()->GetColorAttachmentRendererID()),
					 m_ViewportSize, ImVec2(0, 1), ImVec2(1, 0));

		auto viewportBottumLeft = ImGui::GetCursorScreenPos();
		m_ViewportBottumLeft = { viewportBottumLeft.x, viewportBottumLeft.y };
		

		// Guizmos
		if (HierachyPanel::s_SelectedEntity && m_GizmoType != -1) {
			ImGuizmo::SetOrthographic(false);
			ImGuizmo::SetDrawlist();
			ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y,
							  (float)ImGui::GetWindowWidth(), (float)ImGui::GetWindowHeight());

			Rendering::Camera& cam = Engine::ECS()->GetEcsWorld().GetCamera();
			float aspect = (float)Engine::Rendering()->Window->GetWidth() / (float)Engine::Rendering()->Window->GetHeight();
			if (std::isnan(aspect)) aspect = 1920.f / 1080.f;
			glm::mat4 projection = glm::perspective(glm::radians(cam.Fov), aspect, cam.NearPlane, cam.FarPlane);
			glm::mat4 view = cam.GetViewMatrix();

			auto& selectedTransform = HierachyPanel::s_SelectedEntity.GetComponent<ECS::TransformComponent>();
			glm::mat4 transform = selectedTransform.GetTransform();

			// Snapping
			bool snap = Input::IsKeyDown(KeyCode::LeftControl);
			float snapValue = 0.5f; // Snap to 0.5m for translation/scale
			// Snap to 45 degrees for rotation
			if (m_GizmoType == ImGuizmo::OPERATION::ROTATE)
				snapValue = 45.0f;

			float snapValues[3] = { snapValue, snapValue, snapValue };

			ImGuizmo::Manipulate(glm::value_ptr(view), glm::value_ptr(projection), (ImGuizmo::OPERATION)m_GizmoType, ImGuizmo::LOCAL, glm::value_ptr(transform), nullptr, snap ? snapValues : nullptr);
			if (ImGuizmo::IsUsing()) {
				glm::vec3 position, rotation, scale;
				Math::DecomposeTransform(transform, position, rotation, scale);
				selectedTransform.Position = position;
				selectedTransform.Rotation += rotation - selectedTransform.Rotation;
				selectedTransform.Scale = scale;
			}
		}


		static bool lookAround = false;
		if (ImGui::IsWindowHovered() && Input::IsMouseButtonPressed(MouseButton::Right)) lookAround = true;
		else if (Input::IsMouseButtonReleased(MouseButton::Right)) lookAround = false;
		if (Input::IsControllerInUse()) lookAround = true;

		if (lookAround) {
			glm::vec3 moveDir{ 0,0,0 };
			glm::vec2 moveInput = Input::GetAxisBinding(EDITOR_KEYBINDING_CAM_MOVE);
			moveDir.x = -moveInput.x;
			moveDir.z = moveInput.y;
			if (Input::IsBindingDown(EDITOR_KEYBINDING_CAM_MOVE_UP))   moveDir.y++;
			if (Input::IsBindingDown(EDITOR_KEYBINDING_CAM_MOVE_DOWN)) moveDir.y--;
			glm::vec2 lookDir = Input::GetAxisBinding(EDITOR_KEYBINDING_CAM_LOOK);
			lookDir.y = -lookDir.y;
			SpectatorCamera::Update(&Engine::ECS()->GetEcsWorld().GetCamera(),
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

	static void UIToolbar() {
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 2));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, 0));
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
		auto& colors = ImGui::GetStyle().Colors;
		const auto& buttonHovered = colors[ImGuiCol_ButtonHovered];
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(buttonHovered.x, buttonHovered.y, buttonHovered.z, 0.5f));
		const auto& buttonActive = colors[ImGuiCol_ButtonActive];
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(buttonActive.x, buttonActive.y, buttonActive.z, 0.5f));

		ImGui::Begin("##toolbar", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

		float size = ImGui::GetWindowHeight() - 6.0f;
		ImGui::SetWindowSize(ImVec2(size * 4, size));
		//ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * 0.5f) - (size * 0.5f));

		enum State {
			Edit,
			Play,
			Simulate
		};

		const Rendering::Texture& m_IconPlay = EditorAssetManager::GetIconTexture(Icon::PlayButton);
		const Rendering::Texture& m_IconStop = EditorAssetManager::GetIconTexture(Icon::Stopbutton);
		const Rendering::Texture& m_IconSimulate = EditorAssetManager::GetIconTexture(Icon::SimulateButton);
		const Rendering::Texture& m_IconStep = EditorAssetManager::GetIconTexture(Icon::StepButton);
		const Rendering::Texture& m_IconExit = EditorAssetManager::GetIconTexture(Icon::ExitButton);

		static State state = Edit;
		static bool paused = false;


		switch (state) {
		default:
		case Edit:
			if (UI::ImageButton(m_IconPlay, { size, size })) { state = Play; paused = false; }
			ImGui::SameLine();
			if (UI::ImageButton(m_IconSimulate, { size, size })) { state = Simulate; paused = false; }
			break;
		case Play:
			if (UI::ImageButton(paused ? m_IconPlay : m_IconStop, { size, size })) { paused = !paused; }
			ImGui::SameLine();
			if (UI::ImageButton(m_IconExit, ImVec2(size, size))) { state = Edit; paused = false; }
			if (paused) {
				ImGui::SameLine();
				UI::ImageButton(m_IconStep, { size, size });
			}
			break;
		case Simulate:
			ImGui::Dummy(ImVec2(size, size));
			ImGui::SameLine();
			if (UI::ImageButton(m_IconStop, { size, size })) { state = Edit; }
			if (paused) {
				ImGui::SameLine();
				UI::ImageButton(m_IconStep, { size, size });
			}
			break;
		}

		ImGui::PopStyleVar(2);
		ImGui::PopStyleColor(3);
		ImGui::End();
	}

	void SceneViewportPanel::OnUpdate() {
		if (Input::IsMouseButtonPressed(MouseButton::Left) && !ImGuizmo::IsOver((ImGuizmo::OPERATION)m_GizmoType)) {
			auto absoluteMousePos = ImGui::GetMousePos();
			int mouseX = (int)(absoluteMousePos.x - m_ViewportBottumLeft.x);
			int mouseY = -(int)(absoluteMousePos.y - m_ViewportBottumLeft.y);

			if (mouseX >= 0 && mouseY >= 0 && mouseX < (int)m_ViewportSize.x && mouseY < (int)m_ViewportSize.y) {
				auto& framebufferSpec = Engine::Rendering()->Renderer->GetScreenFramebuffer()->GetSpecification();
				mouseX = (int)((framebufferSpec.Width / m_ViewportSize.x) * mouseX);
				mouseY = (int)((framebufferSpec.Height / m_ViewportSize.y) * mouseY);
				Engine::Rendering()->Renderer->GetScreenFramebuffer()->Bind();
				int pixelData = Engine::Rendering()->Renderer->GetScreenFramebuffer()->ReadPixel(1, mouseX, mouseY);
				Engine::Rendering()->Renderer->GetScreenFramebuffer()->Unbind();
				HierachyPanel::s_SelectedEntity = pixelData == -1 ? ECS::EntityHandle((ECS::Scene*)nullptr, (entt::entity)- 1) : ECS::EntityHandle(&Engine::ECS()->GetEcsWorld(), (entt::entity)pixelData);
			}
		}

		if (!Input::IsMouseButtonDown(MouseButton::Right) && !ImGuizmo::IsUsing()) {
			if (Input::IsKeyPressed(KeyCode::Q)) m_GizmoType = -1;
			if (Input::IsKeyPressed(KeyCode::W)) m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;
			if (Input::IsKeyPressed(KeyCode::E)) m_GizmoType = ImGuizmo::OPERATION::ROTATE;
			if (Input::IsKeyPressed(KeyCode::R) &&
				!Input::IsKeyDown(KeyCode::LeftControl)) m_GizmoType = ImGuizmo::OPERATION::SCALE;
		}
	}
}