#include "pch.hpp"
#include "EditorApplication.hpp"

using namespace Pit;
using namespace Editor;

int main() {
	std::ifstream file;
	file.open("C:/temp.txt");
	file.close();

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