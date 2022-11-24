#pragma once

#include "Core/CoreInclude.hpp"
#include <filesystem>

namespace Pit::FileSystem {
	inline std::string GetRootDir() {
		static std::string rootDir = std::filesystem::current_path().string() + '/';
		return rootDir;
	}

	inline std::string GetConfigDir() {
		return "Config/";
	}

	inline std::string GetEngineDir() {
		return "Engine/";
	}

	inline std::string GetSandboxDir() {
		return "Sandbox/";
	}
}