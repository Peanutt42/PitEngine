#include "pch.hpp"
#include "EditorApplication.hpp"

using namespace Pit;
using namespace Editor;

int main(const int argc, const char* argv[]) {
	setup_crash_handler();

	Engine::CreateInfo engineInfo(argc, argv, "PitEngine-Editor", true, false, true);
	Engine::Init(engineInfo);
	
	EditorApplication::Init();

	while (!Engine::ShouldClose()) {
		std::chrono::time_point<std::chrono::high_resolution_clock> now = std::chrono::high_resolution_clock::now();
		EditorApplication::Update();
		Engine::Update();
	}

	EditorApplication::Shutdown();
	Engine::Shutdown();

	return 0;
}