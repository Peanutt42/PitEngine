#pragma once

#include <string>

namespace Pit {
	/// <summary>
	/// Handles Message prompts 
	/// </summary>
	namespace MessagePrompts {
		void InfoMessage(const wchar_t* title, const wchar_t* msg);
		void ErrorMessage(const wchar_t* title, const wchar_t* msg);
	}

	/// <summary>
	/// Simplifies the running of external programs
	/// </summary>
	namespace Process {
		void Run(const wchar_t* path);
	}
}