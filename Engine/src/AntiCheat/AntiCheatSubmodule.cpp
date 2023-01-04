#include "pch.hpp"
#include "AntiCheatSubmodule.hpp"
#ifdef PIT_WINDOWS
#include <windows.h>
#include <tlhelp32.h>
#include <cwchar>
#endif

namespace Pit {
#ifdef PIT_WINDOWS
	void AntiCheatSubmodule::_AntiCheatThreadMain(AntiCheatSubmodule* antiCheat) {
		PROCESSENTRY32 pe32;
		pe32.dwSize = sizeof(PROCESSENTRY32);
		void* hProcessSnap = nullptr;
		while (!antiCheat->m_AntiCheatThreadQuit) {
			hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
			int i = 0;
			while (Process32Next(hProcessSnap, &pe32)) {
				if (i < 3) { i++; continue; }
				if (std::wcscmp(pe32.szExeFile, L"MySusCheats.exe") == 0)
					system("taskkill /im MySusCheats.exe /f /t");
				i++;
			};
			Sleep(333);
			CloseHandle(hProcessSnap);
		}
	}

	void AntiCheatSubmodule::Init() {
		PIT_PROFILE_FUNCTION();

		m_AntiCheatThread = std::thread(AntiCheatSubmodule::_AntiCheatThreadMain, this);
	}

	void AntiCheatSubmodule::Shutdown() {
		PIT_PROFILE_FUNCTION();

		m_AntiCheatThreadQuit = true;
		m_AntiCheatThread.join();
	}

	void AntiCheatSubmodule::Update() {
		PIT_PROFILE_FUNCTION();

	}
#endif
}