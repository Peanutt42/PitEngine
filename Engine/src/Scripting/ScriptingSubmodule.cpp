#include "pch.hpp"
#include "ScriptingSubmodule.hpp"

#include "mono\jit\jit.h"
#include "mono\metadata\assembly.h"
#include "mono\metadata\object.h"

namespace Pit {
	struct ScriptEngineData {
		MonoDomain* RootDomain = nullptr;
		MonoDomain* AppDomain = nullptr;

		MonoAssembly* CoreAssembly = nullptr;
	};
	static ScriptEngineData* s_Data;


	void ScriptingSubmodule::Init() {
		s_Data = new ScriptEngineData();

		InitMono();
	}

	void ScriptingSubmodule::Shutdown() {
		ShutdownMono();

		delete s_Data;
	}

	void ScriptingSubmodule::Update() {

	}
	

	char* ReadBytes(const std::string& filepath, uint32_t* outSize) {
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

	MonoAssembly* LoadCSharpAssembly(const std::string& assemblyPath) {
		uint32_t fileSize = 0;
		char* fileData = ReadBytes(assemblyPath, &fileSize);

		// NOTE: We can't use this image for anything other than loading the assembly because this image doesn't have a reference to the assembly
		MonoImageOpenStatus status;
		MonoImage* image = mono_image_open_from_data_full(fileData, fileSize, 1, &status, 0);

		if (status != MONO_IMAGE_OK) {
			PIT_ENGINE_ERR(Scripting, "[Mono] Image loading error: {}", mono_image_strerror(status));
			return nullptr;
		}

		MonoAssembly* assembly = mono_assembly_load_from_full(image, assemblyPath.c_str(), &status, 0);
		mono_image_close(image);

		// Don't forget to free the file data
		delete[] fileData;

		return assembly;
	}

	void PrintAssemblyTypes(MonoAssembly* assembly) {
		MonoImage* image = mono_assembly_get_image(assembly);
		const MonoTableInfo* typeDefinitionsTable = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);
		int32_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);

		for (int32_t i = 0; i < numTypes; i++) {
			uint32_t cols[MONO_TYPEDEF_SIZE];
			mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);

			const char* nameSpace = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAMESPACE]);
			const char* name = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAME]);

			PIT_ENGINE_INFO(Scripting, "{}.{}", nameSpace, name);
		}
	}
	
	void ScriptingSubmodule::InitMono() {
		PIT_ENGINE_INFO(Scripting, "Initializing Mono");

		mono_set_assemblies_path("Engine/lib");

		s_Data->RootDomain = mono_jit_init("PitEngineJITRuntime");
		PIT_ENGINE_ASSERT(Scripting, s_Data->RootDomain, "Failed to init mono-jit (mono_jit_init(\"PitEngineJITRuntime\"))");

		s_Data->AppDomain = mono_domain_create_appdomain(String("PitScriptRuntime").data(), nullptr);
		mono_domain_set(s_Data->RootDomain, true);

		s_Data->CoreAssembly = LoadCSharpAssembly("Editor/Resources/Scripts/Engine-ScriptCore.dll");
		PrintAssemblyTypes(s_Data->CoreAssembly);


		MonoImage* assemblyImage = mono_assembly_get_image(s_Data->CoreAssembly);

		MonoClass* monoClass = mono_class_from_name(assemblyImage, "Pit.Scripting", "Main");

		MonoObject* instance = mono_object_new(s_Data->AppDomain, monoClass);
		mono_runtime_object_init(instance);

		MonoMethod* printMsgFunc = mono_class_get_method_from_name(monoClass, "PrintMessage", 0);
		mono_runtime_invoke(printMsgFunc, instance, nullptr, nullptr);

		MonoMethod* printIntMsgFunc = mono_class_get_method_from_name(monoClass, "PrintIntsMessage", 2);
		int value1 = 5, value2 = 15;
		void* params[] = { &value1, &value2 };
		mono_runtime_invoke(printIntMsgFunc, instance, params, nullptr);

		MonoString* string = mono_string_new(s_Data->AppDomain, "Hello World from C++");
		MonoMethod* printCustomMsgFunc = mono_class_get_method_from_name(monoClass, "PrintCustomMessage", 1);
		void* stringParams[] = { string };
		mono_runtime_invoke(printCustomMsgFunc, instance, stringParams, nullptr);
	}

	void ScriptingSubmodule::ShutdownMono() {
		PIT_ENGINE_INFO(Scripting, "Shutting down Mono");

		mono_domain_unload(s_Data->AppDomain);
		s_Data->AppDomain = nullptr;
		mono_jit_cleanup(s_Data->RootDomain);
		s_Data->RootDomain = nullptr;
	}
}