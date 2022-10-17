﻿#include "pch.hpp"
#include "EditorApplication.hpp"

using namespace Pit;
using namespace Editor;

int main() {
	try {
		Engine engine;
		engine.Init();

		EditorApplication editor(engine);
		editor.Init();

		while (!engine.ShouldClose()) {
			editor.Update();
			engine.Update();
		}

		editor.Shutdown();
		engine.Shutdown();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
	}


	return 0;
}