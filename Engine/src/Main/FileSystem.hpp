#pragma once

#include "Main/CoreInclude.hpp"
#include <filesystem>

namespace Pit::FileSystem {
	inline void Init() {
		static bool changedRootTo_PitEngine = false;
		if (changedRootTo_PitEngine) return;
		auto root = std::filesystem::current_path();
		//std::filesystem::current_path(root.parent_path());
		changedRootTo_PitEngine = true;
	}

	inline std::string& GetRootDir() {
		static std::string rootDir = std::filesystem::current_path().string();
		return rootDir;
	}

	inline std::string GetEngineDir() {
		return "Engine/";
	}

#ifdef PIT_EDITOR
	inline std::string GetEditorDir() {
		return "Editor/";
	}
#endif

	inline std::string GetSandboxDir() {
		return "Sandbox/";
	}
}