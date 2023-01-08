#pragma once

#include "Engine.hpp"

#ifdef PIT_WINDOWS

namespace Pit {
	struct ApplicationInfo {
		String Prefix;
		String WindowName;
		bool OneInstanceOnly = false;
	};

	ApplicationInfo GetApplicationInfo();
	void SetupApplication(const int argc, const char* argv[]);
}
extern Pit::ApplicationInfo Pit::GetApplicationInfo();
extern void Pit::SetupApplication(const int argc, const char* argv[]);

int main(const int argc, const char* argv[]) {
	Pit::ApplicationInfo appInfo = Pit::GetApplicationInfo();
	Pit::EngineSettings settings(argc, argv, appInfo.Prefix, appInfo.WindowName, true, false, appInfo.OneInstanceOnly);
	Pit::Engine::Init(settings);
	Pit::SetupApplication(argc, argv);

	while (!Pit::Engine::ShouldClose()) {
		Pit::Engine::Update();
	}

	Pit::Engine::Shutdown();
}

#endif