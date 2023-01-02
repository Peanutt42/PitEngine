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
		return OperatingSystem::Type::Windows10; // TODO
		/* This thinks im using Windows 8...
		if (IsWindows10OrGreater()) return OperatingSystem::Type::Windows10;
		if (IsWindows8Point1OrGreater()) return OperatingSystem::Type::Windows8_1;
		if (IsWindows8OrGreater()) return OperatingSystem::Type::Windows8;
		if (IsWindows7OrGreater()) return OperatingSystem::Type::Windows7;
		if (IsWindowsVistaOrGreater()) return OperatingSystem::Type::WindowsVista;
		if (IsWindowsXPOrGreater()) return OperatingSystem::Type::WindowsXP;
		return OperatingSystem::Type::None;*/
	}

	std::string OperatingSystem::ToString(OperatingSystem::Type sys) {
		switch (sys) {
		default:
		case OperatingSystem::Type::None: return "None";
		case OperatingSystem::Type::WindowsXP: return "Windows-XP";
		case OperatingSystem::Type::WindowsVista: return "Windows-Vista";
		case OperatingSystem::Type::Windows7: return "Windows7";
		case OperatingSystem::Type::Windows8: return "Windows8";
		case OperatingSystem::Type::Windows8_1: return "Windows8.1";
		case OperatingSystem::Type::Windows10: return "Windows10";
		case OperatingSystem::Type::Windows11: return "Windows11";
		}
	}

	const size_t PhysicalStats::GetRam() {
		size_t output = 0;
		if (!GetPhysicallyInstalledSystemMemory(&output)) PIT_ENGINE_ERR(General, "Failed to get physical ram size!");
		return output;
	}
}
#endif