#include "pch.hpp"
#include "EditorApplication.hpp"

using namespace Pit;
using namespace Editor;

int main() {
	Engine::CreateInfo engineInfo{ "PitEngine - Editor", true };
	Engine engine(engineInfo);
	engine.Init();

	EditorApplication editor(engine);
	editor.Init();

	while (!engine.ShouldClose()) {
		std::chrono::time_point<std::chrono::high_resolution_clock> now = std::chrono::high_resolution_clock::now();
		editor.Update();
		engine.Update();

		// editor sleeps when not in use
		
	}

	editor.Shutdown();
	engine.Shutdown();

	return 0;
}