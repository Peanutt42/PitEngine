#include "pch.hpp"
#include "Core/Engine.hpp"


int main(const int argc, const char* argv[]) {
	using namespace Pit;
	
	Engine::CreateInfo engineInfo(argc, argv, "PitEngine-Sandbox", false, false, false);

	Engine::Init(engineInfo);
	
	while (!Engine::ShouldClose()) {
		Engine::Update();
	}

	Engine::Shutdown();

	return 0;
}