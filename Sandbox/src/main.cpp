#include "pch.hpp"
#include "Core/Engine.hpp"
#include "ECS/ECSScene.hpp"
#include "Rendering/RenderingSystem.hpp"
#include "Rendering/Renderer.hpp"
#include "Rendering/Camera.hpp"
#include "Rendering/SpectatorCamera.hpp"

#include "Core/EntryPoint.hpp"



namespace Pit {
	class GameLayer : public Layer {
	public:
		GameLayer() : Layer("GameLayer") {}

		virtual void OnCreate() override {
			Input::SetAxisBinding("Move", KeyAxis(KeyCode::W, KeyCode::S, KeyCode::A, KeyCode::D));
			Input::AddMouseAxisBinding("Look");
			Input::AddBinding("MoveUp", KeyCode::E);
			Input::AddBinding("MoveDown", KeyCode::Q);
			Input::AddBinding("MoveFaster", KeyCode::LeftShift);
			Input::AddBinding("MoveSlower", KeyCode::LeftControl);
			if (Input::GetControllerAxisBinding("Move") == ControllerAxis::None)
				Input::SetAxisBinding("Move", ControllerAxis::Left);
			if (Input::GetControllerAxisBinding("Look") == ControllerAxis::None)
				Input::SetAxisBinding("Look", ControllerAxis::Right);
			if (Input::GetControllerButtonBinding("MoveUp") == ControllerButton::None)
				Input::SetBinding("MoveUp", ControllerButton::Down);
			if (Input::GetControllerButtonBinding("MoveDown") == ControllerButton::None)
				Input::SetBinding("MoveDown", ControllerButton::Right);
			if (Input::GetControllerButtonBinding("MoveFaster") == ControllerButton::None)
				Input::SetBinding("MoveFaster", ControllerButton::LeftStickPress);
			if (Input::GetControllerButtonBinding("MoveSlower") == ControllerButton::None)
				Input::SetBinding("MoveSlower", ControllerButton::RightStickPress);

			Rendering::RenderingSystem::Setup();
		}

		virtual void OnDestroy() override {
			Rendering::RenderingSystem::Shutdown();
		}

		virtual void OnUpdate() override {
			if (Input::IsKeyboardAndMouseInUse() && Input::IsMouseButtonDown(MouseButton::Right) ||
				Input::IsControllerInUse()) {
				glm::vec3 moveDir{ 0,0,0 };
				glm::vec2 moveInput = Input::GetAxisBinding("Move");
				moveDir.x = -moveInput.x;
				moveDir.z = moveInput.y;
				if (Input::IsBindingDown("MoveUp")) moveDir.y++;
				if (Input::IsBindingDown("MoveDown")) moveDir.y--;
				SpectatorCamera::Update(&Engine::GetScene()->GetCamera(),
										moveDir,
										Input::GetAxisBinding("Look") * glm::vec2(1, -1),
										Input::IsBindingDown("MoveFaster"),
										Input::IsBindingDown("MoveSlower"));
			}
		}

		virtual void OnRender() override {
			Rendering::Renderer::BeginScene();

			Rendering::RenderingSystem::RenderScene(*Engine::GetScene());

			Rendering::Renderer::EndScene();
		}
	};

	ApplicationInfo GetApplicationInfo() {
		return ApplicationInfo{ .Prefix = "Game", .WindowName = "PitEngine-Sandbox", .OneInstanceOnly = true };
	}

	void SetupApplication([[maybe_unused]] const int argc, [[maybe_unused]] const char* argv[]) {
		Engine::AddLayer(new GameLayer());
	}
}

/*
int main(const int argc, const char* argv[]) {
	using namespace Pit;
	
	EngineSettings engineSettings(argc, argv, "Game", "PitEngine-Sandbox", true, false, false);

	if (!Engine::Init(engineSettings)) return 0;

	Input::SetAxisBinding("Move", KeyAxis(KeyCode::W, KeyCode::S, KeyCode::A, KeyCode::D));
	Input::AddMouseAxisBinding("Look");
	Input::AddBinding("MoveUp", KeyCode::E);
	Input::AddBinding("MoveDown", KeyCode::Q);
	Input::AddBinding("MoveFaster", KeyCode::LeftShift);
	Input::AddBinding("MoveSlower", KeyCode::LeftControl);
	if (Input::GetControllerAxisBinding("Move") == ControllerAxis::None)
		Input::SetAxisBinding("Move", ControllerAxis::Left);
	if (Input::GetControllerAxisBinding("Look") == ControllerAxis::None)
		Input::SetAxisBinding("Look", ControllerAxis::Right);
	if (Input::GetControllerButtonBinding("MoveUp") == ControllerButton::None)
		Input::SetBinding("MoveUp", ControllerButton::Down);
	if (Input::GetControllerButtonBinding("MoveDown") == ControllerButton::None)
		Input::SetBinding("MoveDown", ControllerButton::Right);
	if (Input::GetControllerButtonBinding("MoveFaster") == ControllerButton::None)
		Input::SetBinding("MoveFaster", ControllerButton::LeftStickPress);
	if (Input::GetControllerButtonBinding("MoveSlower") == ControllerButton::None)
		Input::SetBinding("MoveSlower", ControllerButton::RightStickPress);



	while (!Engine::ShouldClose()) {
		Engine::Update();
		if (Input::IsKeyboardAndMouseInUse() && Input::IsMouseButtonDown(MouseButton::Right) ||
			Input::IsControllerInUse()) {
			glm::vec3 moveDir{ 0,0,0 };
			glm::vec2 moveInput = Input::GetAxisBinding("Move");
			moveDir.x = -moveInput.x;
			moveDir.z = moveInput.y;
			if (Input::IsBindingDown("MoveUp")) moveDir.y++;
			if (Input::IsBindingDown("MoveDown")) moveDir.y--;
			SpectatorCamera::Update(&Engine::GetScene()->GetCamera(),
									moveDir,
									Input::GetAxisBinding("Look") * glm::vec2(1, -1),
									Input::IsBindingDown("MoveFaster"),
									Input::IsBindingDown("MoveSlower"));
		}
	}

	Engine::Shutdown();

	return 0;
}*/