﻿#include "pch.hpp"
#include "EditorApplication.hpp"
#include "Serialization\YAMLSerializer.hpp"

using namespace Pit;
using namespace Editor;

int main(const int argc, const char* argv[]) {
	String projDir = "Sandbox/Sandbox.pitproj";
	for (int i = 1; i < argc; i++) {
		if (String(argv[i]) == "-proj" && argc < i + 1)
			projDir = argv[i + 1];
	}
	PIT_ENGINE_ASSERT(Editor, std::filesystem::exists(projDir), "The project specified doesn't exists!");

	Serialization::YamlDeserializer in(projDir);
	String projName;
	in.Find("Name", projName);
	String projEngineVersionStr;
	in.Find("EngineVersion", projEngineVersionStr);
	ProjectInfo projInfo{ .Name = projName, .EngineVersion = Version(projEngineVersionStr) };
	PIT_ENGINE_ASSERT(Editor, projInfo.EngineVersion == Engine::Version, "Project {0:s}'s version ({1}) is not compatible with this version of the Editor({2})", projName, projInfo.EngineVersion, Engine::Version);


	EngineSettings engineSettings(argc, argv, "Editor", "PitEngine-Editor : " + projName, true, true, false, true);

	if (!Engine::Init(engineSettings)) return 0;

	EditorApplication::SelectProject(projInfo);
	EditorApplication::Init();
	
	while (!Engine::ShouldClose()) {
		EditorApplication::Update();
		Engine::Update();
	}

	EditorApplication::Shutdown();
	Engine::Shutdown();

	return 0;
}