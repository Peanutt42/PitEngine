#include "pch.hpp"
#include "ScriptUtils.hpp"
#include "Core/Engine.hpp"
#include "ScriptingSubmodule.hpp"

#include <mono/metadata/class.h>
#include <mono/metadata/object.h>

namespace Pit::Scripting {
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