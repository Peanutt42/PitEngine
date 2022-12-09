﻿#include "pch.hpp"
#include "EditorApplication.hpp"

using namespace Pit;
using namespace Editor;

int main(const int argc, const char* argv[]) {
	EngineSettings engineSettings(argc, argv, FileSystem::GetConfigDir() + "EditorEngineConfig.ini", "PitEngine-Editor", true, true, false);
	if (!Engine::Init(engineSettings)) return 0;
	
	EditorApplication::Init();

	while (!Engine::ShouldClose()) {
		EditorApplication::Update();
		Engine::Update();
	}

	EditorApplication::Shutdown();
	Engine::Shutdown();

	return 0;
}