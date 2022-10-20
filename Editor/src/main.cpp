#include "pch.hpp"
#include "EditorApplication.hpp"

using namespace Pit;
using namespace Editor;

int main() {
	Engine engine;
	engine.Init();

	EditorApplication editor(engine);
	editor.Init();

	while (!engine.ShouldClose()) {
		std::chrono::time_point<std::chrono::high_resolution_clock> now = std::chrono::high_resolution_clock::now();
		editor.Update();
		engine.Update();

		// editor sleeps when not in use
		if (Engine::Rendering()->GetRenderer()->Window.IsMinimized() ||
			!Engine::Rendering()->GetRenderer()->Window.IsFocused() && !Engine::Rendering()->GetRenderer()->Window.IsHovered())
			std::this_thread::sleep_for(16ms);
	}

	editor.Shutdown();
	engine.Shutdown();

	return 0;
}