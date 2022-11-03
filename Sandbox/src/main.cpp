#include "pch.hpp"
#include "Rendering/Window.hpp"
#include "Debug/vcrash.h"

using namespace Pit;

int main(const int argc, const char* argv[]) {
	Array<String> consoleArgs;
	for (int i = 0; i < argc; i++) consoleArgs.push_back(argv[i]);

	setup_crash_handler();

	Engine::CreateEngine(consoleArgs, "PitEngine - Sandbox", true, false, false);
	
	while (!Engine::ShouldClose()) {
		Engine::Update();
	}

	Engine::Shutdown();

	return 0;
}