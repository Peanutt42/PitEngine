#include "pch.hpp"
#include "ScriptingSubmodule.hpp"
#include "Core/Engine.hpp"

#include "ScriptGlue.hpp"
#include "ScriptUtils.hpp"

#include <mono/jit/jit.h>
#include "mono/metadata/assembly.h"
#include "mono/metadata/object.h"
#include "mono/metadata/tabledefs.h"
#include "mono/metadata/metadata.h"
#include "mono/metadata/mono-debug.h"
#include "mono/metadata/threads.h"

#include <glm/glm.hpp>

#include <filewatch/FileWatch.hpp>

namespace Pit {
	using namespace Scripting;

	static std::unordered_map<std::string, ScriptFieldType> s_ScriptFieldTypeMap = {
		{ "System.Single", ScriptFieldType::Float },
		{ "System.Double", ScriptFieldType::Double },
		{ "System.Boolean", ScriptFieldType::Bool },
		{ "System.Char", ScriptFieldType::Char },
		{ "System.Int16", ScriptFieldType::Short },
		{ "System.Int32", ScriptFieldType::Int },
		{ "System.Int64", ScriptFieldType::Long },
		{ "System.Byte", ScriptFieldType::Byte },
		{ "System.UInt16", ScriptFieldType::UShort },
		{ "System.UInt32", ScriptFieldType::UInt },
		{ "System.UInt64", ScriptFieldType::ULong }
	};

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
	
		static ScriptFieldType MonoTypeToScriptFieldType(MonoType* monoType) {
			std::string typeName = mono_type_get_name(monoType);

			auto it = s_ScriptFieldTypeMap.find(typeName);
			if (it == s_ScriptFieldTypeMap.end()) {
				PIT_ENGINE_ERR(Scripting, "Unknown type: {}", typeName);
				return ScriptFieldType::None;
			}

			return it->second;
		}
	
		static inline const char* ScriptFieldTypeToString(ScriptFieldType fieldType) {
			switch (fieldType) {
			case ScriptFieldType::None:    return "None";
			case ScriptFieldType::Float:   return "Float";
			case ScriptFieldType::Double:  return "Double";
			case ScriptFieldType::Bool:    return "Bool";
			case ScriptFieldType::Char:    return "Char";
			case ScriptFieldType::Byte:    return "Byte";
			case ScriptFieldType::Short:   return "Short";
			case ScriptFieldType::Int:     return "Int";
			case ScriptFieldType::Long:    return "Long";
			case ScriptFieldType::UByte:   return "UByte";
			case ScriptFieldType::UShort:  return "UShort";
			case ScriptFieldType::UInt:    return "UInt";
			case ScriptFieldType::ULong:   return "ULong";
			}
			PIT_ENGINE_FATAL(Scripting, "Unknown ScriptFieldType");
			return "None";
		}

		static inline ScriptFieldType ScriptFieldTypeFromString(std::string_view fieldType) {
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
	}


	struct ScriptEngineData {
		MonoDomain* MonoRootDomain = nullptr; // Contains domain for mono
		MonoDomain* AppDomain = nullptr;

		MonoAssembly* CoreAssembly = nullptr;
		MonoImage* CoreAssemblyImage = nullptr;
		std::filesystem::path CoreAssemblyFilepath;

		MonoAssembly* AppAssembly = nullptr;
		MonoImage* AppAssemblyImage = nullptr;
		std::filesystem::path AppAssemblyFilepath;
		bool AppAssemblyReloadPending = false;
		ScopeRef<filewatch::FileWatch<String>> AppAssemblyFileWatcher;

		bool EnableDebugging = false;

		std::unordered_map<String, ScriptClass> ComponentClasses;
		std::unordered_map<String, ScriptClass> SystemClasses;
	};
	static ScriptEngineData* s_Data;

	static void OnAppAssemblyFileSystemEvent([[maybe_unused]] const String& path, const filewatch::Event event) {
		if (!s_Data->AppAssemblyReloadPending && event == filewatch::Event::modified) {
			s_Data->AppAssemblyReloadPending = true;
			Engine::SubmitToMainThread([]() {
				if (s_Data->AppAssemblyReloadPending) {
					Engine::Scripting()->ReloadAssembly();
					s_Data->AppAssemblyReloadPending = false;
				}
			});
		}
	}

	void ScriptingSubmodule::Init() {
		PIT_PROFILE_FUNCTION();

		s_Data = new ScriptEngineData();

		InitMono();
		Scripting::ScriptGlue::RegisterCoreFunctions();
		if (!LoadCoreAssembly("Engine-ScriptCore/Binaries/Engine-ScriptCore.dll")) {
			PIT_ENGINE_ERR(Scripting, "Failed to load 'Engine-ScriptCore.dll' assembly");
			return;
		}
		if (!LoadAppAssembly("Sandbox/Scripts/Binaries/Sandbox.dll")) {
			PIT_ENGINE_ERR(Scripting, "Failed to load 'Sandbox.dll' assembly");
			return;
		}
		LoadAssemblyClasses();

		s_Data->AppAssemblyFileWatcher = CreateScopeRef<filewatch::FileWatch<String>>(
			s_Data->AppAssemblyFilepath.string(),
			OnAppAssemblyFileSystemEvent
			);


		// ### TESTING ###
		ScriptClass systemClass("Sandbox", "TestSystem");
		MonoMethod* updateSystemMethod = systemClass.GetMethod("Update", 0);
		ScriptInstance systemInstance(systemClass.GetNative());
		systemInstance.Invoke(updateSystemMethod, nullptr);

		ScriptClass mainClass = ScriptClass("Sandbox", "Main");
		ScriptInstance mainInstance(mainClass.GetNative());
		MonoMethod* printCustomMsgFunc = mainClass.GetMethod("PrintCustomMessage", 1);
		MonoString* string = mono_string_new(s_Data->AppDomain, "Hi, this was invoked by C++ and printed in C#!");
		void* params[] = { string };
		mainInstance.Invoke(printCustomMsgFunc, params);
	}

	void ScriptingSubmodule::Shutdown() {
		PIT_PROFILE_FUNCTION();

		ShutdownMono();

		delete s_Data;
	}

	void ScriptingSubmodule::Update() {
	}

	void ScriptingSubmodule::InitMono() {
		PIT_PROFILE_FUNCTION();

		PIT_ENGINE_INFO(Scripting, "Initializing Mono");

		if (std::filesystem::exists("JSON")) std::filesystem::remove_all("JSON"); // Random folder

		mono_set_assemblies_path("Engine-ScriptCore/lib");

		if (s_Data->EnableDebugging) {
			const char* argv[2] = {
				"--debugger-agent=transport=dt_socket,address=127.0.0.1:2550,server=y,suspend=n,loglevel=3,logfile=Logs/MonoDebugger.log",
				"--soft-breakpoints"
			};
			mono_jit_parse_options(2, (char**)argv);
			mono_debug_init(MONO_DEBUG_FORMAT_MONO);
		}

		s_Data->MonoRootDomain = mono_jit_init("PitEngineJITRuntime");
		PIT_ENGINE_ASSERT(Scripting, s_Data->MonoRootDomain, "Failed to init mono-jit (mono_jit_init(\"PitEngineJITRuntime\"))");

		if (s_Data->EnableDebugging)
			mono_debug_domain_create(s_Data->MonoRootDomain);

		mono_thread_set_main(mono_thread_current());
	}

	void ScriptingSubmodule::ShutdownMono() {
		PIT_PROFILE_FUNCTION();

		PIT_ENGINE_INFO(Scripting, "Shutting down Mono");

		mono_domain_set(mono_get_root_domain(), false);

		mono_domain_unload(s_Data->AppDomain);
		s_Data->AppDomain = nullptr;

		mono_jit_cleanup(s_Data->MonoRootDomain);
		s_Data->MonoRootDomain = nullptr;
	}

	const bool ScriptingSubmodule::LoadCoreAssembly(const std::filesystem::path& binaryFilepath) {
		PIT_PROFILE_FUNCTION();

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

		PIT_ENGINE_INFO(Scripting, "Successfully loaded CoreAssembly '{}'", binaryFilepath.string());
		return true;
	}

	const bool ScriptingSubmodule::LoadAppAssembly(const std::filesystem::path& binaryFilepath) {
		PIT_PROFILE_FUNCTION();

		s_Data->AppAssemblyFilepath = binaryFilepath;
		s_Data->AppAssembly = Utils::LoadMonoAssembly(binaryFilepath, s_Data->EnableDebugging);
		if (!s_Data->AppAssembly) {
			PIT_ENGINE_ERR(Scripting, "Failed to load AppAssembly with file '{}'", binaryFilepath.string());
			return false;
		}

		s_Data->AppAssemblyImage = mono_assembly_get_image(s_Data->AppAssembly);
		if (!s_Data->AppAssemblyImage) {
			PIT_ENGINE_ERR(Scripting, "Failed to get mono image of the AppAssembly! file: '{}'", binaryFilepath.string());
			return false;
		}

		PIT_ENGINE_INFO(Scripting, "Successfully loaded AppAssembly '{}'", binaryFilepath.string());
		return true;
	}

	void ScriptingSubmodule::ReloadAssembly() {
		PIT_PROFILE_FUNCTION();

		PIT_ENGINE_INFO(Scripting, "Reloading changed Assemblies");

		ScopedTimer t("Script assembly reloading");

		mono_domain_set(mono_get_root_domain(), false);

		mono_domain_unload(s_Data->AppDomain);

		LoadCoreAssembly(s_Data->CoreAssemblyFilepath);
		LoadAppAssembly(s_Data->AppAssemblyFilepath);

		LoadAssemblyClasses();
	}

	void ScriptingSubmodule::LoadAssemblyClasses() {
		PIT_PROFILE_FUNCTION();

		s_Data->SystemClasses.clear();
		s_Data->ComponentClasses.clear();

		const MonoTableInfo* typeDefinitionsTable = mono_image_get_table_info(s_Data->AppAssemblyImage, MONO_TABLE_TYPEDEF);
		int32_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);

		// Get Internal types from Engine-ScriptCore
		MonoClass* systemClass = mono_class_from_name(s_Data->CoreAssemblyImage, "PitEngine.ECS", "System");

		PIT_ENGINE_INFO(Scripting, "Reflection metadata:");
		for (int32_t i = 1; i < numTypes; i++) {
			uint32_t cols[MONO_TYPEDEF_SIZE];
			mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);

			const char* nameSpace = mono_metadata_string_heap(s_Data->AppAssemblyImage, cols[MONO_TYPEDEF_NAMESPACE]);
			const char* name = mono_metadata_string_heap(s_Data->AppAssemblyImage, cols[MONO_TYPEDEF_NAME]);
			String fullName;
			if (strlen(nameSpace) > 0) fullName = std::format("{}.{}", nameSpace, name);
			else fullName = name;
			PIT_ENGINE_INFO(Scripting, "{}", fullName);

			MonoClass* monoClass = mono_class_from_name(s_Data->AppAssemblyImage, nameSpace, name);


			// Methods
			int methodCount = mono_class_num_methods(monoClass);
			if (methodCount > 0) {
				void* methodIter = nullptr;
				while (MonoMethod* method = mono_class_get_methods(monoClass, &methodIter)) {
					const char* methodName = mono_method_get_name(method);
					PIT_ENGINE_INFO(Scripting, "  {}()", methodName);
				}
			}
			
			bool isSystem = mono_class_is_subclass_of(monoClass, systemClass, false) &&
							mono_class_get_method_from_name(monoClass, "Update", 0) != nullptr;
			if (isSystem)
				s_Data->SystemClasses[fullName] = ScriptClass(nameSpace, name);
			bool isComponent = methodCount <= 1 && !mono_class_is_subclass_of(monoClass, systemClass, false); // 1 constructor, finalizer doesn't count
			if (isComponent)
				s_Data->ComponentClasses[fullName] = ScriptClass(nameSpace, name);
			
			// Fields/Variables
			int fieldCount = mono_class_num_fields(monoClass);
			if (fieldCount > 0) {
				void* fieldIter = nullptr;
				while (MonoClassField* field = mono_class_get_fields(monoClass, &fieldIter)) {
					const char* fieldName = mono_field_get_name(field);
					uint32_t flags = mono_field_get_flags(field);
					bool isPublic = flags & FIELD_ATTRIBUTE_PUBLIC;
					bool isPrivate = flags & FIELD_ATTRIBUTE_PRIVATE;
					
					MonoType* type = mono_field_get_type(field);
					ScriptFieldType fieldType = Utils::MonoTypeToScriptFieldType(type);
					PIT_ENGINE_INFO(Scripting, "  {} {} {}", isPublic ? "public" : (isPrivate ? "private" : ""), Utils::ScriptFieldTypeToString(fieldType), fieldName);

					// TODO: scriptClass.m_Fields[fieldName] = { fieldType, fieldName, field };
				}
			}
		}

		ScriptClass testMainClass("Sandbox", "Main");
		MonoMethod* testMainPrintFunc = testMainClass.GetMethod("PrintCustomMessage", 1);
		ScriptInstance testMainInstance(testMainClass.GetNative());
		MonoString* string = mono_string_new(s_Data->AppDomain, "AfterReload Msg!");
		void* params[] = { string };
		testMainInstance.Invoke(testMainPrintFunc, params);
	}

	MonoImage* ScriptingSubmodule::GetCoreAssemblyImage() { return s_Data->CoreAssemblyImage; }
	MonoImage* ScriptingSubmodule::GetAppAssemblyImage() { return s_Data->AppAssemblyImage; }
	
	const bool ScriptingSubmodule::ComponentClassExists(const String& className) const { return s_Data->ComponentClasses.find(className) != s_Data->ComponentClasses.end(); }

	MonoObject* ScriptingSubmodule::InstantiateClass(MonoClass* monoClass) {
		MonoObject* obj = mono_object_new(s_Data->AppDomain, monoClass);
		mono_runtime_object_init(obj);
		return obj;
	}
	

	
}