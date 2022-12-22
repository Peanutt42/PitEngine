#include "pch.hpp"
#include "AntiCheatSubmodule.hpp"
#ifdef PIT_WINDOWS
#include <windows.h>
#include <tlhelp32.h>
#include <tchar.h>
#endif

namespace Pit {
#ifdef PIT_WINDOWS
	void AntiCheatSubmodule::_AntiCheatThreadMain(AntiCheatSubmodule* antiCheat) {
		HANDLE hProcessSnap;
		[[maybe_unused]] HANDLE hProcess;
		PROCESSENTRY32 pe32;
		pe32.dwSize = sizeof(PROCESSENTRY32);
		[[maybe_unused]] DWORD dwPriorityClass;
		hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		while (!antiCheat->m_AntiCheatThreadQuit) {
			Array<std::wstring> processNames;
			processNames.reserve(200);
			hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
			int i = 0;
			while (Process32Next(hProcessSnap, &pe32)) {
				if (i < 3) { i++; continue; }
				std::wstring temp;
				for (auto c : pe32.szExeFile) {
					if (c == '\0') break;
					temp += c;
				}
				processNames.push_back(temp);
				i++;
			};
			for (auto& proc_name : processNames) {
				if (proc_name == L"MySusCheats.exe") PIT_ENGINE_FATAL(General, "Sussy cheats detected");
			}
			Sleep(333);
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