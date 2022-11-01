#include "pch.hpp"
#include "Rendering/Window.hpp"

using namespace Pit;

int main() {
	Engine::CreateInfo engineInfo { 
		.WindowName = "PitEngine - Sandbox",
		.WindowToolbar = true
	};
	Engine engine(engineInfo);
	engine.Init();
	while (!engine.ShouldClose()) {
		engine.Update();
	}

	engine.Shutdown();

	return 0;
}