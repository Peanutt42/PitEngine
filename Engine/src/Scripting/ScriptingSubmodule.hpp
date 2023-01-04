#pragma once

#include "Core/CoreInclude.hpp"
#include "Core/Submodule.hpp"

extern "C" {
	typedef struct _MonoObject MonoObject;
	typedef struct _MonoClass MonoClass;
	typedef struct _MonoMethod MonoMethod;
	typedef struct _MonoImage MonoImage;
	typedef struct _MonoDomain MonoDomain;
	typedef struct _MonoAssembly MonoAssembly;
}

namespace Pit {
	class ScriptClass {
	public:
		ScriptClass() = default;
		ScriptClass(const String& classNamespace, const String& className);

		MonoMethod* GetMethod(const String& name, int paramCount);

		MonoObject* Instantiate();

		MonoClass* GetNative();

	private:
		String m_ClassNamespace, m_ClassName;
		MonoClass* m_MonoClass = nullptr;
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

	class ScriptingSubmodule : public Submodule {
	public:
		virtual void Init() override;
		virtual void Shutdown() override;
		virtual void Update() override;

		const bool LoadAssembly(const std::filesystem::path& binaryFilepath);

		const bool ComponentClassExists(const String& className) const;

		MonoImage* GetCoreAssemblyImage();

	private:
		void InitMono(), ShutdownMono();

		static MonoObject* InstantiateClass(MonoClass* monoClass);
		void LoadAssemblyMetadata(MonoAssembly* assembly);

		friend class ScriptClass;
		friend class ScriptInstance;
	};
}