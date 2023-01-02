#include "pch.hpp"
#include "Platform/PlatformUtils.hpp"
#include "Core/Engine.hpp"

#ifdef PIT_WINDOWS
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include <windows.h>
#include <VersionHelpers.h>
#include <tlhelp32.h>

namespace Pit {

	void MessagePrompts::InfoMessage(const wchar_t* title, const wchar_t* msg) {
		MessageBoxW(NULL, msg, title, MB_USERICON | MB_OK);
	}

	void MessagePrompts::ErrorMessage(const wchar_t* title, const wchar_t* msg) {
		MessageBoxW(NULL, msg, title, MB_ICONERROR | MB_OK);
	}

	void Process::Run(const wchar_t* path) {
		STARTUPINFO si;
		ZeroMemory(&si, sizeof(si));
		si.cb = sizeof(si);
		PROCESS_INFORMATION pi;
		ZeroMemory(&pi, sizeof(pi));

		CreateProcess(path, nullptr, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
	}

	const unsigned long Process::GetCurrentProcessID() { return GetCurrentProcessId(); }

	const std::wstring Process::GetName(unsigned long processId) {
		HANDLE hProcessSnap;
		PROCESSENTRY32 pe32;
		pe32.dwSize = sizeof(PROCESSENTRY32);
		hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		int i = 0;
		while (Process32Next(hProcessSnap, &pe32)) {
			if (i < 3) { i++; continue; }
			i++;
			if (pe32.th32ProcessID != processId) continue;
			std::wstring name;
			for (auto c : pe32.szExeFile) {
				if (c == '\0') break;
				name += c;
			}
			return name;
		};
		return L"notfound";
	}

	std::thread::id Thread::MainThreadId = std::this_thread::get_id();


	OperatingSystem::Type OperatingSystem::Get() {
		#ifdef PIT_WINDOWS
			#ifdef _WIN64
				return OperatingSystem::Type::Win64;
			#else
				return OperatingSystem::Type::Win32;
			#endif
		#elif defined(PIT_LINUX)
			return OperatingSystem::Type::Linux;
		#elif defined(PIT_MACOS)
		return OperatingSystem::Type::MacOS;
		#endif
	}

	std::string OperatingSystem::ToString(OperatingSystem::Type sys) {
		switch (sys) {
		default:
		case OperatingSystem::Type::None: return "None";
		case OperatingSystem::Type::Win32: return "Win32";
		case OperatingSystem::Type::Win64: return "Win64";
		case OperatingSystem::Type::MacOS: return "MacOS";
		case OperatingSystem::Type::Linux: return "Linux";
		}
	}

	const size_t PhysicalStats::GetRam() {
		size_t output = 0;
		if (!GetPhysicallyInstalledSystemMemory(&output)) PIT_ENGINE_ERR(General, "Failed to get physical ram size!");
		return output;
	}
}
#endif