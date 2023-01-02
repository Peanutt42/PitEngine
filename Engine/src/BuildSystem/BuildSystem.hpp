#pragma once

#include "Core\CoreInclude.hpp"
#include "Platform\PlatformUtils.hpp"

namespace Pit {
	enum class BuildConfiguration {
		Debug,
		Release
	};
	static constexpr String BuildConfigurationToString(BuildConfiguration config) {
		switch (config) {
		default:
		case BuildConfiguration::Debug: return "Debug";
		case BuildConfiguration::Release: return "Release";
		}
	}

	struct BuildReport {
		bool Successful;
		OperatingSystem::Type OS;
		BuildConfiguration Config;
		int TotalBuildTimeSeconds;

		void Print();
	};

	class BuildSystem {
	public:
		static BuildReport BuildGame(OperatingSystem::Type os, BuildConfiguration config);
	};
}