#pragma once

#include "CoreInclude.h"
#include <filesystem>

namespace Pit::FileSystem {
	inline void Init() {
		static bool changedRootTo_PitEngine = false;
		if (changedRootTo_PitEngine) return;
		auto root = std::filesystem::current_path();
#if true
		std::filesystem::current_path(root.parent_path().parent_path());
#endif
		changedRootTo_PitEngine = true;
	}

	inline std::string& GetRootDir() {
		static std::string rootDir = std::filesystem::current_path().string();
		return rootDir;
	}

	inline std::string& GetEngineDir() {
		static std::string engineDir = GetRootDir() + std::string("/Engine");
		return engineDir;
	}

	inline std::string& GetSandboxDir() {
		static std::string sandboxDir = GetRootDir() + std::string("/Sandbox");
		return sandboxDir;
	}
}