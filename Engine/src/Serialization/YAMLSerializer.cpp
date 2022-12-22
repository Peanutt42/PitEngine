#include "pch.hpp"
#include "YAMLSerializer.hpp"

namespace Pit::Serialization {
	const bool YamlSerializer::SaveToFile(const String& filepath) {
		std::ofstream fout(filepath);
		if (!fout.is_open()) return false;

		fout << m_Emitter.c_str();
		fout.close();
		return true;
	}
	
	const char* YamlSerializer::GetString() const {
		return m_Emitter.c_str();
	}
	
	
	YamlDeserializer::YamlDeserializer(const String& filepath) {
		if (!std::filesystem::exists(filepath)) PIT_ENGINE_FATAL(Serialization, "YamlDeserializer can't open this file {}", filepath);

		std::ifstream fin(filepath);
		if (!fin.is_open()) PIT_ENGINE_FATAL(Serialization, "YamlDeserializer can't open this file {}", filepath);

		std::stringstream strStream;
		strStream << fin.rdbuf();
		fin.close();

		m_Data = YAML::Load(strStream.str());
	}
}