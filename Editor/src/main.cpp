#include "pch.hpp"
#include "EditorApplication.hpp"

using namespace Pit;
using namespace Editor;

int main(const int argc, const char* argv[]) {
	Array<String> consoleArgs;
	for (int i = 0; i < argc; i++) consoleArgs.push_back(argv[i]);

	Engine::CreateEngine(consoleArgs, "PitEngine - Editor", true, false, true);
	
	EditorApplication editor;
	editor.Init();

	while (!Engine::ShouldClose()) {
		std::chrono::time_point<std::chrono::high_resolution_clock> now = std::chrono::high_resolution_clock::now();
		editor.Update();
		Engine::Update();
	}

	editor.Shutdown();
	Engine::Shutdown();

	return 0;
}