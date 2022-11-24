#include "pch.hpp"
#include "Core/Engine.hpp"


int main(const int argc, const char* argv[]) {
	using namespace Pit;
	
	EngineSettings engineSettings(argc, argv, FileSystem::GetConfigDir() + "SandboxEngineConfig.ini", "PitEngine-Sandbox", true, false, false);

	Engine::Init(engineSettings);
	
	while (!Engine::ShouldClose()) {
		Engine::Update();
	}

	Engine::Shutdown();

	return 0;
}