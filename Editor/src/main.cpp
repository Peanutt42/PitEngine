#include "EditorApplication.hpp"

using namespace Pit;
using namespace Editor;

int main() {
	Engine engine;
	engine.Init();

	EditorApplication editor(engine);
	editor.Init();

	while (!engine.ShouldClose()) {
		editor.Update();
		engine.Update();
	}

	editor.Shutdown();
	engine.Shutdown();


	return 0;
}