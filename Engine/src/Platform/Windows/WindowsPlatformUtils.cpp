#include "pch.hpp"
#ifdef PIT_WINDOWS

#include "Platform/PlatformUtils.hpp"
#include "Core/Engine.hpp"
#include <commdlg.h>
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include <windows.h>

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

}
#endif