#pragma once

#include "Core/CoreInclude.hpp"
#include "Core/Submodule.hpp"
#include "ScriptUtils.hpp"

extern "C" {
	typedef struct _MonoObject MonoObject;
	typedef struct _MonoClass MonoClass;
	typedef struct _MonoImage MonoImage;
}

namespace Pit {
	class ScriptingSubmodule : public Submodule {
	public:
		virtual void Init() override;
		virtual void Shutdown() override;
		virtual void Update() override;

		const bool LoadCoreAssembly(const std::filesystem::path& binaryFilepath);
		const bool LoadAppAssembly(const std::filesystem::path& binaryFilepath);
		void ReloadAssembly();

		const bool ComponentClassExists(const String& className) const;
		const Scripting::ScriptClass& GetComponentClass(const String& className) const;

		MonoImage* GetCoreAssemblyImage();
		MonoImage* GetAppAssemblyImage();

	private:
		void InitMono(), ShutdownMono();

		static MonoObject* InstantiateClass(MonoClass* monoClass);
		void LoadAssemblyClasses();

		friend Scripting::ScriptClass;
		friend Scripting::ScriptInstance;
	};
}