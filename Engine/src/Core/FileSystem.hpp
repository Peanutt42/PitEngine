#pragma once

#include <string>
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

	inline char* ReadFileBytes(const std::filesystem::path& filepath, uint32_t* outSize) {
		std::ifstream stream(filepath, std::ios::binary | std::ios::ate);

		if (!stream) return nullptr;

		std::streampos end = stream.tellg();
		stream.seekg(0, std::ios::beg);
		uint32_t size = (uint32_t)(end - stream.tellg());

		if (size == 0) return nullptr;

		char* buffer = new char[size];
		stream.read((char*)buffer, size);
		stream.close();

		*outSize = size;
		return buffer;
	}
}