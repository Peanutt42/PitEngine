#pragma once

#include "Core/CoreInclude.hpp"

namespace Pit {
	enum class RenderingAPI {
		None,
		OpenGL
	};

	struct EngineSettings {
		Array<String> ConsoleArgs;
		String WindowName;
		bool WindowToolbar;
		bool VSync;
		bool UIDocking;
		RenderingAPI RenderingApi;
		int AntiAliasing;
		bool Headless;

		EngineSettings(const int argc, const char* argv[], const String& windowName, bool windowToolbar, bool vsync, bool uiDocking, RenderingAPI rendereringApi = RenderingAPI::OpenGL, int antiAliasing = 0, bool headless = false)
			: WindowName(windowName), WindowToolbar(windowToolbar), VSync(vsync), UIDocking(uiDocking), RenderingApi(rendereringApi), AntiAliasing(antiAliasing), Headless(headless) {

			ConsoleArgs.assign(argv, argv + argc);
		}
	};
}