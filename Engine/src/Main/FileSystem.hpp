#pragma once

#include "Main/CoreInclude.hpp"
#include <filesystem>

namespace Pit::FileSystem {
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