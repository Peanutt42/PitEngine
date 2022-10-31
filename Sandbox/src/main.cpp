#include "pch.hpp"
#include "Rendering/Window.hpp"
#include "Utils/PlatformUtils.hpp"

using namespace Pit;

int main() {
	MessagePrompts::InfoMessage("");
	MessagePrompts::ErrorMessage();
	Engine::CreateInfo engineInfo{ "PitEngine - Sandbox", false };
	Engine engine(engineInfo);
	engine.Init();
	while (!engine.ShouldClose()) {
		engine.Update();
	}

	engine.Shutdown();

	return 0;
}