#pragma once

#include <string>
#ifdef PIT_WINDOWS
#include <windows.h>
#endif

namespace Pit {
	/// <summary>
	/// Handles Message prompts 
	/// </summary>
	class MessagePrompts {
	public:
#ifdef PIT_WINDOWS
		static void InfoMessage(LPCWSTR msg);
		static void ErrorMessage(LPCWSTR msg);
#endif
	};

	/// <summary>
	/// Simplifies the running of external programs
	/// </summary>
	class Process {
	public:
#ifdef PIT_WINDOWS
		static void Run(LPCWSTR path);
#endif
	};
}