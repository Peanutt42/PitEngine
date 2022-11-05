#include "pch.hpp"
#include "EditorApplication.hpp"

using namespace Pit;
using namespace Editor;

int main(const int argc, const char* argv[]) {
	Engine::CreateInfo engineInfo(argc, argv, "PitEngine-Editor", true, false, true);
	Engine::Init(engineInfo);
	
	EditorApplication::Init();

	while (!Engine::ShouldClose()) {
		EditorApplication::Update();
		Engine::Update();
	}

	EditorApplication::Shutdown();
	Engine::Shutdown();

	return 0;
}