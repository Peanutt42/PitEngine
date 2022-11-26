#include "pch.hpp"
#include "EditorApplication.hpp"

using namespace Pit;
using namespace Editor;

struct Test {
	int i;
};

int main(const int argc, const char* argv[]) {
	Ref<Test> myIntPtr;
	if (myIntPtr) {

	}
	else if (!myIntPtr) {
		myIntPtr = new Test();
	}
	myIntPtr->i = 0;



	EngineSettings engineSettings(argc, argv, FileSystem::GetConfigDir() + "EditorEngineConfig.ini", "PitEngine-Editor", true, true, false);
	Engine::Init(engineSettings);
	
	EditorApplication::Init();

	while (!Engine::ShouldClose()) {
		EditorApplication::Update();
		Engine::Update();
	}

	EditorApplication::Shutdown();
	Engine::Shutdown();

	return 0;
}