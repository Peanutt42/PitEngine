#include "pch.hpp"
#include "Rendering/Window.hpp"

using namespace Pit;

int main() {
	Engine::CreateInfo engineInfo{ "PitEngine - Sandbox", false };
	Engine engine(engineInfo);
	engine.Init();

	while (!engine.ShouldClose()) {
		engine.Update();
	}

	engine.Shutdown();

	return 0;
}