#include "pch.hpp"
#include "KeyValueSerialization.hpp"

namespace Pit::Serialization {
	KeyValueSerialization::KeyValueSerialization(const String& filepath)
		: m_Fout(filepath) {

		PIT_ENGINE_ASSERT(Serialization, m_Fout.is_open(), "Failed to create file {}", filepath);
	}
	
	KeyValueSerialization::~KeyValueSerialization() {
		if (m_Fout.is_open()) m_Fout.close();
	}
	
	void KeyValueSerialization::AddKeyValue(const String& key, const String& value) {
		m_Fout << key << ": " << value << '\n';
	}
	
	void KeyValueSerialization::Save() { m_Fout.close(); }
	
	
	
	void KeyValueDeserialization::Deserialize(const String& filepath, Array<KeyValuePair>& keyValuePairs) {
		PIT_ENGINE_ASSERT(Serialization, std::filesystem::exists(filepath), "Trying to deserialize file: {} that ", filepath);

		std::ifstream fin(filepath);
		PIT_ENGINE_ASSERT(Serialization, fin.is_open(), "Failed to open file {}", filepath);

		String line;
		int i = 0;
		keyValuePairs.clear();
		keyValuePairs.emplace_back();
		while (getline(fin, line)) {
			bool keyFinished = false;
			bool skipSpace = false;
			for (auto c : line) {
				if (skipSpace && c == ' ')			{ skipSpace = false; continue; }
				if (c == 0)							break;
				if (c == ':')						{ keyFinished = true; skipSpace = true; }
				else if (keyFinished)				keyValuePairs[i].Value += c;
				else								keyValuePairs[i].Key += c;
			}
			keyValuePairs.emplace_back();
			i++;
		}
		fin.close();
	}
}