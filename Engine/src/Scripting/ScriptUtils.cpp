#include "pch.hpp"
#include "ScriptUtils.hpp"
#include "Core/Engine.hpp"
#include "ScriptingSubmodule.hpp"

#include <mono/metadata/class.h>
#include <mono/metadata/object.h>

namespace Pit::Scripting {
	static std::unordered_map<std::string, ScriptFieldType> s_ScriptFieldTypeMap = {
		{ "System.Single",	ScriptFieldType::Float },
		{ "System.Double",	ScriptFieldType::Double },
		{ "System.Boolean",	ScriptFieldType::Bool },
		{ "System.Char",	ScriptFieldType::Char },
		{ "System.Int16",	ScriptFieldType::Short },
		{ "System.Int32",	ScriptFieldType::Int },
		{ "System.Int64",	ScriptFieldType::Long },
		{ "System.Byte",	ScriptFieldType::Byte },
		{ "System.UInt16",	ScriptFieldType::UShort },
		{ "System.UInt32",	ScriptFieldType::UInt },
		{ "System.UInt64",	ScriptFieldType::ULong }
	};

	ScriptFieldType MonoTypeToScriptFieldType(MonoType* monoType) {
		std::string typeName = mono_type_get_name(monoType);

		auto it = s_ScriptFieldTypeMap.find(typeName);
		if (it == s_ScriptFieldTypeMap.end()) {
			PIT_ENGINE_ERR(Scripting, "Unknown type: {}", typeName);
			return ScriptFieldType::None;
		}

		return it->second;
	}

	const char* ScriptFieldTypeToString(ScriptFieldType fieldType) {
		switch (fieldType) {
		default:
		case ScriptFieldType::None:    return "unknown";
		case ScriptFieldType::Float:   return "float";
		case ScriptFieldType::Double:  return "double";
		case ScriptFieldType::Bool:    return "bool";
		case ScriptFieldType::Char:    return "char";
		case ScriptFieldType::Byte:    return "byte";
		case ScriptFieldType::Short:   return "short";
		case ScriptFieldType::Int:     return "int";
		case ScriptFieldType::Long:    return "long";
		case ScriptFieldType::UByte:   return "ubyte";
		case ScriptFieldType::UShort:  return "ushort";
		case ScriptFieldType::UInt:    return "uint";
		case ScriptFieldType::ULong:   return "ulong";
		}
	}

	ScriptFieldType ScriptFieldTypeFromString(std::string_view fieldType) {
		if (fieldType == "None")    return ScriptFieldType::None;
		if (fieldType == "Float")   return ScriptFieldType::Float;
		if (fieldType == "Double")  return ScriptFieldType::Double;
		if (fieldType == "Bool")    return ScriptFieldType::Bool;
		if (fieldType == "Char")    return ScriptFieldType::Char;
		if (fieldType == "Byte")    return ScriptFieldType::Byte;
		if (fieldType == "Short")   return ScriptFieldType::Short;
		if (fieldType == "Int")     return ScriptFieldType::Int;
		if (fieldType == "Long")    return ScriptFieldType::Long;
		if (fieldType == "UByte")   return ScriptFieldType::UByte;
		if (fieldType == "UShort")  return ScriptFieldType::UShort;
		if (fieldType == "UInt")    return ScriptFieldType::UInt;
		if (fieldType == "ULong")   return ScriptFieldType::ULong;

		PIT_ENGINE_FATAL(Scripting, "Unknown ScriptFieldType");
		return ScriptFieldType::None;
	}

	// ScriptClass
	ScriptClass::ScriptClass(const String& classNamespace, const String& className, bool isCore)
		: m_ClassNamespace(classNamespace), m_ClassName(className) {

		PIT_PROFILE_FUNCTION();

		m_MonoClass = mono_class_from_name(isCore ? Engine::Scripting()->GetCoreAssemblyImage() : Engine::Scripting()->GetAppAssemblyImage(), classNamespace.c_str(), className.c_str());
		if (!m_MonoClass) PIT_ENGINE_WARN(Scripting, "Trying to get class '{}.{}' that doesn't exists!", classNamespace, className);
	}

	MonoMethod* ScriptClass::GetMethod(const String& name, int paramCount) {
		PIT_PROFILE_FUNCTION();

		MonoMethod* method = mono_class_get_method_from_name(m_MonoClass, name.c_str(), paramCount);
		if (!method) PIT_ENGINE_WARN(Scripting, "Trying to get a method '{}' of class '{}.{}' that doesn't exists!", name, m_ClassNamespace, m_ClassName);
		return method;
	}

	MonoObject* ScriptClass::Instantiate() {
		return Engine::Scripting()->InstantiateClass(m_MonoClass);
	}

	MonoClass* ScriptClass::GetNative() { return m_MonoClass; }


	// ScriptInstance
	ScriptInstance::ScriptInstance(MonoClass* scriptClass) : m_Class(scriptClass) {
		if (!scriptClass) PIT_ENGINE_ERR(Scripting, "Trying to make a ScriptInstance of a nullptr scriptClass!");
		m_Instance = Engine::Scripting()->InstantiateClass(scriptClass);
	}

	void ScriptInstance::Invoke(MonoMethod* method, void** params) {
		if (!m_Instance) PIT_ENGINE_ERR(Scripting, "Trying to call a function on a ScriptInstance with m_Instance = nullptr!");
		mono_runtime_invoke(method, m_Instance, params, nullptr);
	}

	MonoClass* ScriptInstance::GetClass() { return m_Class; }
}