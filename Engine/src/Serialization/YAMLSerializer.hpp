#pragma once

#include "Core/CoreInclude.hpp"
#include <yaml-cpp/yaml.h>

namespace Pit::Serialization {
	class YamlSerializer {
	public:
		#pragma region YamlSerializer overloading
		inline YamlSerializer& operator<<(const std::string& v) {
			m_Emitter.Write(v);
			return *this;
		}
		inline YamlSerializer& operator<<(bool v) {
			m_Emitter.Write(v);
			return *this;
		}
		inline YamlSerializer& operator<<(char v) {
			m_Emitter.Write(v);
			return *this;
		}
		inline YamlSerializer& operator<<(unsigned char v) {
			m_Emitter.Write(static_cast<char>(v));
			return *this;
		}
		inline YamlSerializer& operator<<(const YAML::_Alias& v) {
			m_Emitter.Write(v);
			return *this;
		}
		inline YamlSerializer& operator<<(const YAML::_Anchor& v) {
			m_Emitter.Write(v);
			return *this;
		}
		inline YamlSerializer& operator<<(const YAML::_Tag& v) {
			m_Emitter.Write(v);
			return *this;
		}
		inline YamlSerializer& operator<<(const YAML::_Comment& v) {
			m_Emitter.Write(v);
			return *this;
		}
		inline YamlSerializer& operator<<(const YAML::_Null& v) {
			m_Emitter.Write(v);
			return *this;
		}
		inline YamlSerializer& operator<<(const YAML::Binary& b) {
			m_Emitter.Write(b);
			return *this;
		}

		inline YamlSerializer& operator<<(const char* v) {
			m_Emitter.Write(std::string(v));
			return *this;
		}

		inline YamlSerializer& operator<<(int v) {
			m_Emitter.WriteIntegralType(v);
			return *this;
		}
		inline YamlSerializer& operator<<(unsigned int v) {
			m_Emitter.WriteIntegralType(v);
			return *this;
		}
		inline YamlSerializer& operator<<(short v) {
			m_Emitter.WriteIntegralType(v);
			return *this;
		}
		inline YamlSerializer& operator<<(unsigned short v) {
			m_Emitter.WriteIntegralType(v);
			return *this;
		}
		inline YamlSerializer& operator<<(long v) {
			m_Emitter.WriteIntegralType(v);
			return *this;
		}
		inline YamlSerializer& operator<<(unsigned long v) {
			m_Emitter.WriteIntegralType(v);
			return *this;
		}
		inline YamlSerializer& operator<<(long long v) {
			m_Emitter.WriteIntegralType(v);
			return *this;
		}
		inline YamlSerializer& operator<<(unsigned long long v) {
			m_Emitter.WriteIntegralType(v);
			return *this;
		}

		inline YamlSerializer& operator<<(float v) {
			m_Emitter.WriteStreamable(v);
			return *this;
		}
		inline YamlSerializer& operator<<(double v) {
			m_Emitter.WriteStreamable(v);
			return *this;
		}

		inline YamlSerializer& operator<<(YAML::EMITTER_MANIP value) {
			m_Emitter.SetLocalValue(value);
			return *this;
		}

		inline YamlSerializer& operator<<(YAML::_Indent indent) {
			m_Emitter.SetLocalIndent(indent);
			return *this;
		}

		inline YamlSerializer& operator<<(YAML::_Precision precision) {
			m_Emitter.SetLocalPrecision(precision);
			return *this;
		}
#pragma endregion

		const bool SaveToFile(const String& filepath);

		const char* GetString() const;

	private:
		YAML::Emitter m_Emitter;
	};



	class YamlDeserializer {
	public:
		YamlDeserializer(const String& filepath);

		template<typename T>
		bool Find(const String& name, T& output) {
			YAML::Node node = m_Data[name];
			if (node) {
				output = node.as<T>();
				return true;
			}
			else return false;
		}

	private:
		YAML::Node m_Data;
	};
}