#include "pch.hpp"
#include "Platform/PlatformUtils.hpp"
#include "Core/Engine.hpp"
#include "Rendering/Window.hpp"

#ifdef PIT_WINDOWS
#include <GLFW/glfw3.h>
#pragma warning(push)
#pragma warning(disable: 4005)
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#pragma warning(pop)
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

	// copied from TheCherno/Hazel https://github.com/TheCherno/Hazel/blob/master/Hazel/src/Platform/Windows/WindowsPlatformUtils.cpp
	const std::string FileDialogs::OpenFile(const char* filter) {
		OPENFILENAMEA ofn;
		ZeroMemory(&ofn, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME);
		if (Engine::GetWindow())
			ofn.hwndOwner = glfwGetWin32Window(Engine::GetWindow()->GetWindowHandle());
		else
			ofn.hwndOwner = nullptr;
		char szFile[260] = { 0 };
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = filter;
		ofn.nFilterIndex = 1;
		ofn.Flags = OFN_PATHMUSTEXIST |
					OFN_FILEMUSTEXIST |
					OFN_NOCHANGEDIR;

		if (GetOpenFileNameA(&ofn))
			return ofn.lpstrFile;

		return std::string();
	}

	// copied from TheCherno/Hazel https://github.com/TheCherno/Hazel/blob/master/Hazel/src/Platform/Windows/WindowsPlatformUtils.cpp
	const std::string FileDialogs::SaveFile(const char* filter) {
		OPENFILENAMEA ofn;
		CHAR szFile[260] = { 0 };
		ZeroMemory(&ofn, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME);
		if (Engine::GetWindow())
			ofn.hwndOwner = glfwGetWin32Window(Engine::GetWindow()->GetWindowHandle());
		else
			ofn.hwndOwner = nullptr;
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = filter;
		ofn.nFilterIndex = 1;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;
		ofn.lpstrDefExt = strchr(filter, '\0') + 1;

		if (GetSaveFileNameA(&ofn) == TRUE)
			return ofn.lpstrFile;

		return std::string();
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
		PROCESSENTRY32 pe32;
		pe32.dwSize = sizeof(PROCESSENTRY32);
		HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		int i = 0;
		while (Process32Next(hProcessSnap, &pe32)) {
			if (i < 3) { i++; continue; }
			i++;
			if (pe32.th32ProcessID != processId) continue;
			CloseHandle(hProcessSnap);
			return std::wstring(pe32.szExeFile);
		};
		return L"";
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