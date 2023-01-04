#include "pch.hpp"
#include "ScriptGlue.hpp"

#include "mono/metadata/object.h"

namespace Pit::Scripting {
	#define PIT_ADD_INTERNAL_CALL(name) mono_add_internal_call("PitEngine.InternalCalls::" #name, (const void*)name)
	
	static void Log(MonoString* msg) {
		char* str = mono_string_to_utf8(msg);
		PIT_ENGINE_INFO(Scripting, "[Game]: {}", str);
		mono_free(str);
	}

	void ScriptGlue::RegisterCoreFunctions() {
		PIT_ADD_INTERNAL_CALL(Log);
	}
}