#pragma once

#include "CoreInclude.hpp"
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
	inline const char* GetRootDir(const std::string& extra) {
		static std::string rootDir = std::filesystem::current_path().string();
		const char* dir = (rootDir + extra).c_str();
		return dir;
	}

	inline std::string& GetEngineDir() {
		static std::string engineDir = GetRootDir() + std::string("/Engine/");
		return engineDir;
	}
	inline const char* GetEngineDir(const std::string& extra) {
		static std::string engineDir = GetRootDir() + std::string("/Engine/");
		const char* dir = (engineDir + extra).c_str();
		return dir;
	}

	inline std::string& GetSandboxDir() {
		static std::string sandboxDir = GetRootDir() + std::string("/Sandbox/");
		return sandboxDir;
	}
	inline const char* GetSandboxDir(const std::string& extra) {
		static std::string sandboxDir = GetRootDir() + std::string("/Sandbox/");
		const char* dir = (sandboxDir + extra).c_str();
		return dir;
	}
}