#include "pch.hpp"
#include "ScriptingSubmodule.hpp"

#include "Core/Engine.hpp"

#include "ScriptGlue.hpp"

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/object.h>
#include <mono/metadata/metadata.h>
#include <mono/metadata/reflection.h>
#include <mono/metadata/tabledefs.h>
#include <mono/metadata/mono-debug.h>
#include <mono/metadata/threads.h>

#include <glm/glm.hpp>

namespace Pit {
	namespace Utils {
		// TODO: Maybe create a custom File class with such helper functions
		static char* ReadBytes(const std::filesystem::path& filepath, uint32_t* outSize) {
			std::ifstream stream(filepath, std::ios::binary | std::ios::ate);

			if (!stream) return nullptr;

			std::streampos end = stream.tellg();
			stream.seekg(0, std::ios::beg);
			uint32_t size = Cast<uint32_t>(end - stream.tellg());

			if (size == 0) return nullptr;

			char* buffer = new char[size];
			stream.read((char*)buffer, size);
			stream.close();

			*outSize = size;
			return buffer;
		}

		static MonoAssembly* LoadMonoAssembly(const std::filesystem::path& assemblyPath, bool loadPDB = false) {
			uint32_t fileSize = 0;
			char* fileData = ReadBytes(assemblyPath, &fileSize);

			// NOTE: We can't use this image for anything other than loading the assembly because this image doesn't have a reference to the assembly
			MonoImageOpenStatus status;
			MonoImage* image = mono_image_open_from_data_full(fileData, fileSize, 1, &status, 0);

			if (status != MONO_IMAGE_OK) {
				PIT_ENGINE_ERR(Scripting, "[Mono] Image loading error: {}", mono_image_strerror(status));
				return nullptr;
			}

			if (loadPDB) {
				std::filesystem::path pdbPath = assemblyPath;
				pdbPath.replace_extension(".pdb");
				std::string pdbPathStr = pdbPath.string();

				if (std::filesystem::exists(pdbPath)) {
					uint32_t pdbFileSize = 0;
					char* pdbFileData = ReadBytes(pdbPathStr, &pdbFileSize);
					mono_debug_open_image_from_memory(image, (const mono_byte*)pdbFileData, pdbFileSize);
					PIT_ENGINE_INFO(Scripting, "Loaded PDB {}", pdbPathStr);
				}
			}

			MonoAssembly* assembly = mono_assembly_load_from_full(image, assemblyPath.string().c_str(), &status, 0);
			mono_image_close(image);

			// Don't forget to free the file data
			delete[] fileData;

			return assembly;
		}
	}


	struct ScriptEngineData {
		MonoDomain* RootDomain = nullptr;
		MonoDomain* AppDomain = nullptr;

		MonoAssembly* CoreAssembly = nullptr;
		MonoImage* CoreAssemblyImage = nullptr;
		std::filesystem::path CoreAssemblyFilepath;

		bool EnableDebugging = false;

		std::unordered_map<String, ScriptClass> ComponentClasses;
		std::unordered_map<String, ScriptClass> SystemClasses;
	};
	static ScriptEngineData* s_Data;


	void ScriptingSubmodule::Init() {
		s_Data = new ScriptEngineData();

		InitMono();
		if (!LoadAssembly("Engine/Resources/Scripts/Engine-ScriptCore.dll")) {
			PIT_ENGINE_ERR(Scripting, "Failed to load 'Engine-ScriptCore.dll' assembly");
			return;
		}
		ScriptClass systemClass("Pit", "System");
		MonoMethod* updateSystemMethod = systemClass.GetMethod("Update", 0);

		LoadAssemblyMetadata(s_Data->CoreAssembly);

		Scripting::ScriptGlue::RegisterFunctions();

		// ### TESTING ###
		ScriptInstance systemInstance(systemClass.GetNative());
		systemInstance.Invoke(updateSystemMethod, nullptr);

		ScriptClass mainClass = ScriptClass("Pit", "Main");
		ScriptInstance mainInstance(mainClass.GetNative());
		MonoMethod* printCustomMsgFunc = mainClass.GetMethod("PrintCustomMessage", 1);
		MonoString* string = mono_string_new(s_Data->AppDomain, "Hi, this was invoked by C++ and printed in C#!");
		void* params[] = { string };
		mainInstance.Invoke(printCustomMsgFunc, params);
	}

	void ScriptingSubmodule::Shutdown() {
		ShutdownMono();

		delete s_Data;
	}

	void ScriptingSubmodule::Update() {
	}

	void ScriptingSubmodule::InitMono() {
		PIT_ENGINE_INFO(Scripting, "Initializing Mono");

		if (std::filesystem::exists("JSON")) std::filesystem::remove("JSON"); // Random folder

		mono_set_assemblies_path("Engine/lib");

		if (s_Data->EnableDebugging) {
			const char* argv[2] = {
				"--debugger-agent=transport=dt_socket,address=127.0.0.1:2550,server=y,suspend=n,loglevel=3,logfile=Logs/MonoDebugger.log",
				"--soft-breakpoints"
			};
			mono_jit_parse_options(2, (char**)argv);
			mono_debug_init(MONO_DEBUG_FORMAT_MONO);
		}

		s_Data->RootDomain = mono_jit_init("PitEngineJITRuntime"); 
		PIT_ENGINE_ASSERT(Scripting, s_Data->RootDomain, "Failed to init mono-jit (mono_jit_init(\"PitEngineJITRuntime\"))");

		if (s_Data->EnableDebugging)
			mono_debug_domain_create(s_Data->RootDomain);

		mono_thread_set_main(mono_thread_current());
	}

	void ScriptingSubmodule::ShutdownMono() {
		PIT_ENGINE_INFO(Scripting, "Shutting down Mono");

		mono_domain_set(mono_get_root_domain(), false);

		mono_domain_unload(s_Data->AppDomain);
		s_Data->AppDomain = nullptr;

		mono_jit_cleanup(s_Data->RootDomain);
		s_Data->RootDomain = nullptr;
	}

	const bool ScriptingSubmodule::LoadAssembly(const std::filesystem::path& binaryFilepath) {
		// Create App-domain
		char appName[] = "PitEngineScriptRuntime";
		s_Data->AppDomain = mono_domain_create_appdomain(appName, nullptr);
		mono_domain_set(s_Data->AppDomain, true);

		s_Data->CoreAssemblyFilepath = binaryFilepath;
		s_Data->CoreAssembly = Utils::LoadMonoAssembly(binaryFilepath, s_Data->EnableDebugging);
		if (!s_Data->CoreAssembly) {
			PIT_ENGINE_ERR(Scripting, "Failed to load CoreAssembly with file '{}'", binaryFilepath.string());
			return false;
		}

		s_Data->CoreAssemblyImage = mono_assembly_get_image(s_Data->CoreAssembly);
		if (!s_Data->CoreAssemblyImage) {
			PIT_ENGINE_ERR(Scripting, "Failed to get mono image of the CoreAssembly! file: '{}'", binaryFilepath.string());
			return false;
		}

		PIT_ENGINE_INFO(Scripting, "Successfully loaded assembly '{}'", binaryFilepath.string());
		return true;
	}

	void ScriptingSubmodule::LoadAssemblyMetadata(MonoAssembly* assembly) {
		s_Data->SystemClasses.clear();
		s_Data->ComponentClasses.clear();

		MonoImage* image = mono_assembly_get_image(assembly);
		const MonoTableInfo* typeDefinitionsTable = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);
		int32_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);

		for (int32_t i = 1; i < numTypes; i++) {
			uint32_t cols[MONO_TYPEDEF_SIZE];
			mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);

			const char* nameSpace = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAMESPACE]);
			const char* name = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAME]);
			String fullName;
			if (strlen(nameSpace) > 0) fullName = std::format("{}.{}", nameSpace, name);
			else fullName = name;

			MonoClass* monoClass = mono_class_from_name(image, nameSpace, name);

			void* iter = nullptr;
			int methodCount = 0;
			while (true) {
				MonoMethod* method = mono_class_get_methods(monoClass, &iter);
				if (!method) break;
				methodCount++;
			}
			bool isSystem = mono_class_get_method_from_name(monoClass, "Update", 0) != nullptr;
			if (isSystem)
				s_Data->SystemClasses[fullName] = ScriptClass(nameSpace, name);
			bool isComponent = methodCount <= 1; // 1 constructor, finalizer doesn't count
			if (isComponent)
				s_Data->ComponentClasses[fullName] = ScriptClass(nameSpace, name);
			
			PIT_ENGINE_INFO(Scripting, "{}.{}", nameSpace, name);
		}
	}

	MonoImage* ScriptingSubmodule::GetCoreAssemblyImage() { return s_Data->CoreAssemblyImage; }
	
	const bool ScriptingSubmodule::ComponentClassExists(const String& className) const { return s_Data->ComponentClasses.find(className) != s_Data->ComponentClasses.end(); }

	MonoObject* ScriptingSubmodule::InstantiateClass(MonoClass* monoClass) {
		MonoObject* obj = mono_object_new(s_Data->AppDomain, monoClass);
		mono_runtime_object_init(obj);
		return obj;
	}
	

	// ScriptClass
	ScriptClass::ScriptClass(const String& classNamespace, const String& className)
		: m_ClassNamespace(classNamespace), m_ClassName(className) {

		PIT_PROFILE_FUNCTION();

		m_MonoClass = mono_class_from_name(Engine::Scripting()->GetCoreAssemblyImage(), classNamespace.c_str(), className.c_str());
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