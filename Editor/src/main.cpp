#define PIT_EDITOR true
#include "Main/Engine.hpp"

using namespace Pit;

int main() {
	Engine engine;
	engine.Init();

	while (!engine.ShouldClose()) {
		engine.Update();
	}

	engine.Shutdown();

	return 0;
}