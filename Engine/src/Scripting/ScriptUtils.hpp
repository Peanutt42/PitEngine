#pragma once

#include "Core/CoreInclude.hpp"

extern "C" {
	typedef struct _MonoClass MonoClass;
	typedef struct _MonoObject MonoObject;
	typedef struct _MonoMethod MonoMethod;
	typedef struct _MonoType MonoType;
	typedef struct _MonoClassField MonoClassField;
}

namespace Pit::Scripting {
	enum class ScriptFieldType {
		None = 0,
		Float, Double,
		Bool, Char, Byte, Short, Int, Long,
		UByte, UShort, UInt, ULong
	};

	struct ScriptField {
		ScriptFieldType Type;
		std::string Name;

		MonoClassField* ClassField;
	};

	ScriptFieldType MonoTypeToScriptFieldType(MonoType* monoType);
	const char* ScriptFieldTypeToString(ScriptFieldType fieldType);
	ScriptFieldType ScriptFieldTypeFromString(std::string_view fieldType);

	// ScriptField + data storage
	struct ScriptFieldInstance {
		ScriptField Field;

		ScriptFieldInstance() {
			memset(m_Buffer, 0, sizeof(m_Buffer));
		}

		template<typename T>
		T GetValue() {
			static_assert(sizeof(T) <= 16, "Type too large!");
			return *(T*)m_Buffer;
		}

		template<typename T>
		void SetValue(T value) {
			static_assert(sizeof(T) <= 16, "Type too large!");
			std::memcpy(m_Buffer, &value, sizeof(T));
		}
	private:
		uint8_t m_Buffer[16];

		friend class ScriptingSubmodule;
		friend class ScriptInstance;
	};

	using ScriptFieldMap = std::unordered_map<std::string, ScriptFieldInstance>;

	class ScriptClass {
	public:
		ScriptClass() = default;
		ScriptClass(const String& classNamespace, const String& className, bool isCore = false);

		MonoMethod* GetMethod(const String& name, int paramCount);

		MonoObject* Instantiate();

		MonoClass* GetNative();

	private:
		String m_ClassNamespace, m_ClassName;
		std::map<std::string, ScriptField> m_Fields;
		MonoClass* m_MonoClass = nullptr;

		friend class ScriptingSubmodule;
	};

	class ScriptInstance {
	public:
		ScriptInstance() = default;
		ScriptInstance(MonoClass* scriptClass);
		// TODO deletion of MonoObject ~ScriptInstance() {}

		void Invoke(MonoMethod* method, void** params);

		MonoClass* GetClass();

	private:
		MonoClass* m_Class = nullptr;
		MonoObject* m_Instance = nullptr;
	};
}