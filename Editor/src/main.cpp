#include "pch.hpp"
#include "EditorApplication.hpp"
#include "Serialization/YAMLSerializer.hpp"
#include "Platform/PlatformUtils.hpp"

using namespace Pit;
using namespace Editor;

int main(const int argc, const char* argv[]) {
	String projDir;
	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-proj") == 0 && argc < i + 1)
			projDir = argv[i + 1];
	}
	while (projDir.empty() || !std::filesystem::exists(projDir)) {
		projDir = FileDialogs::OpenFile("PitEngine Project (*.pitproj)\0*.pitproj\0");
	}

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