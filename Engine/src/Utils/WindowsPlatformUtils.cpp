#include "pch.hpp"
#ifdef PIT_WINDOWS

#include "PlatformUtils.hpp"
#include "Core/Engine.hpp"
#include <commdlg.h>
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include <windows.h>

using namespace Pit;

void MessagePrompts::InfoMessage(LPCWSTR msg) {
	MessageBoxW(NULL, msg, L"PitEngine-Info", MB_USERICON | MB_OK);
}

void MessagePrompts::ErrorMessage(LPCWSTR msg) {
	MessageBox(NULL, msg, L"PitEngine-Error", MB_ICONERROR | MB_OK);
}

#endif