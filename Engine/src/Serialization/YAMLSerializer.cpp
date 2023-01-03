#include "pch.hpp"
#include "YAMLSerializer.hpp"

namespace Pit::Serialization {
	const bool YamlSerializer::SaveToFile(const std::filesystem::path& filepath) {
		std::ofstream fout(filepath);
		if (!fout.is_open()) return false;

		fout << m_Emitter.c_str();
		fout.close();
		return true;
	}
	
	const char* YamlSerializer::GetString() const {
		return m_Emitter.c_str();
	}
	
	
	YamlDeserializer::YamlDeserializer(const std::filesystem::path& filepath) {
		if (!std::filesystem::exists(filepath)) PIT_ENGINE_FATAL(Serialization, "YamlDeserializer can't open this file {}", filepath.string());

		std::ifstream fin(filepath);
		if (!fin.is_open()) PIT_ENGINE_FATAL(Serialization, "YamlDeserializer can't open this file {}", filepath.string());

		std::stringstream strStream;
		strStream << fin.rdbuf();
		fin.close();

		m_Data = YAML::Load(strStream.str());
	}
}