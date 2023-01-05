#pragma once

#include <string>

namespace Pit {
	/// <summary>
	/// Handles Message prompts 
	/// </summary>
	class MessagePrompts {
	public:
		static void InfoMessage(const wchar_t* title, const wchar_t* msg);
		static void ErrorMessage(const wchar_t* title, const wchar_t* msg);
	};


	class FileDialogs {
	public:
		static const std::string OpenFile(const char* filter);
		static const std::string SaveFile(const char* filter);
	};

	/// <summary>
	/// Simplifies the running of external programs
	/// </summary>
	class Process {
	public:
		static void Run(const wchar_t* path);
		static const unsigned long GetCurrentProcessID();
		static const std::wstring GetName(unsigned long processId);
	};

	class Thread {
	public:
		static std::thread::id MainThreadId;
	};

	class OperatingSystem {
	public:
		enum class Type {
			None = 0,
			// Windows
			Win32,
			Win64,
			// MacOS
			MacOS,
			// Linux
			Linux
		};

		static Type Get();
		static std::string ToString(Type sys);
	};

	class PhysicalStats {
	public:
		/// <returns>Physical ram size (in kilobytes)</returns>
		static const size_t GetRam();
	};
}