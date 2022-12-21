#pragma once

#include "Core/CoreInclude.hpp"

namespace Pit::Serialization {
	class KeyValueSerialization {
	public:
		KeyValueSerialization(const String& filepath)
			: m_Fout(filepath) {
			
			PIT_ENGINE_ASSERT(Serialization, m_Fout.is_open(), "Failed to create file {}", filepath);
		}

		~KeyValueSerialization() {
			if (m_Fout.is_open()) m_Fout.close();
		}

		void AddKeyValue(const String& key, const String& value) {
			m_Fout << key << ": " << value << '\n';
		}

		void Save() { m_Fout.close(); }

	private:
		std::ofstream m_Fout;
	};

	class KeyValueDeserialization {
	public:
		struct KeyValuePair {
			String Key;
			String Value;
		};

		static void Deserialize(const String& filepath, Array<KeyValuePair>& keyValuePairs) {
			PIT_ENGINE_ASSERT(Serialization, std::filesystem::exists(filepath), "Trying to deserialize file: {} that ", filepath);

			std::ifstream fin(filepath);
			PIT_ENGINE_ASSERT(Serialization, fin.is_open(), "Failed to open file {}", filepath);

			String line;
			int i = 0;
			keyValuePairs.clear();
			keyValuePairs.emplace_back();
			while (getline(fin, line)) {
				bool keyFinished = false;
				for (auto c : line) {
					if (c == 0)							break;
					if (c == ':')						keyFinished = true;
					else if (keyFinished && c != ' ')	keyValuePairs[i].Value += c;
					else if (c != ' ')					keyValuePairs[i].Key += c;
				}
				keyValuePairs.emplace_back();
				i++;
			}
			fin.close();
		}
	};
}