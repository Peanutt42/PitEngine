#pragma once

#include "Core/CoreInclude.hpp"

namespace Pit::Serialization {
	class KeyValueSerialization {
	public:
		KeyValueSerialization(const String& filepath);
		~KeyValueSerialization();

		void AddKeyValue(const String& key, const String& value);

		void Save();

	private:
		std::ofstream m_Fout;
	};

	class KeyValueDeserialization {
	public:
		struct KeyValuePair {
			String Key;
			String Value;
		};

		static void Deserialize(const String& filepath, Array<KeyValuePair>& keyValuePairs);
	};
}