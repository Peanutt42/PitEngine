#include "pch.hpp"
#include "Core/Engine.hpp"
#include "Rendering/RenderingSubmodule.hpp"
#include "Rendering/Camera.hpp"
#include "Rendering/SpectatorCamera.hpp"


int main(const int argc, const char* argv[]) {
	using namespace Pit;
	
	EngineSettings engineSettings(argc, argv, FileSystem::GetConfigDir() + "SandboxEngineConfig.ini", "PitEngine-Sandbox", true, false, false);

	if (!Engine::Init(engineSettings)) return 0;
	
	Input::AddBinding("MoveForward", KeyCode::W);
	Input::AddBinding("MoveBackward", KeyCode::S);
	Input::AddBinding("MoveLeft", KeyCode::A);
	Input::AddBinding("MoveRight", KeyCode::D);
	Input::AddBinding("MoveUp", KeyCode::E);
	Input::AddBinding("MoveDown", KeyCode::Q);
	Input::AddBinding("MoveFaster", KeyCode::LeftShift);
	Input::AddBinding("MoveSlower", KeyCode::LeftControl);

	while (!Engine::ShouldClose()) {
		Engine::Update();
		if (Input::IsMouseButtonDown(MouseButton::Right))
			SpectatorCamera::Update(Engine::Rendering()->Camera,
									Input::GetBinding("MoveForward"),
									Input::GetBinding("MoveBackward"),
									Input::GetBinding("MoveLeft"),
									Input::GetBinding("MoveRight"),
									Input::GetBinding("MoveUp"),
									Input::GetBinding("MoveDown"),
									Input::GetBinding("MoveFaster"),
									Input::GetBinding("MoveSlower"));
	}

	Engine::Shutdown();

	return 0;
}