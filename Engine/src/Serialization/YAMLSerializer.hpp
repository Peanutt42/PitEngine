#pragma once

#include "Core/CoreInclude.hpp"
#include <yaml-cpp/yaml.h>

namespace Pit::Serialization {
	[[maybe_unused]] static const bool SaveYamlToFile(const std::filesystem::path& filepath, const YAML::Emitter& emitter) {
		std::ofstream fout(filepath);
		if (!fout) {
			PIT_ENGINE_ERR(General, "Failed to load file '{}'", filepath.string());
			return false;
		}

		fout << emitter.c_str();
		fout.close();
		return true;
	}
}