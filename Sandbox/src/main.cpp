#include "pch.hpp"
#include "Rendering/Window.hpp"

using namespace Pit;

int main() {
	Engine engine;
	engine.Init();

	while (!engine.ShouldClose()) {
		engine.Update();
	}

	engine.Shutdown();

	return 0;
}