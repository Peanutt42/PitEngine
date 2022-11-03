#include "pch.hpp"
#include "Rendering/Window.hpp"
#include "Debug/vcrash.h"

using namespace Pit;

int main(const int argc, const char* argv[]) {
	setup_crash_handler();

	Engine::CreateInfo engineInfo(argc, argv, "PitEngine-Sandbox", true, false, false);

	Engine::Init(engineInfo);
	
	while (!Engine::ShouldClose()) {
		Engine::Update();
	}

	Engine::Shutdown();

	return 0;
}