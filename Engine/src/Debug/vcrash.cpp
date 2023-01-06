#include "pch.hpp"
#include "vcrash.h"

#include "Core/Engine.hpp"
#include "Debug/Logging.hpp"
#include "Utils/DateUtils.hpp"
#include "Platform/PlatformUtils.hpp"

#ifdef PIT_WINDOWS
#include <windows.h>
#include <DbgHelp.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

static void ShutdownEngine(int /*sig*/) { Pit::Engine::ForceShutdown(); }

void CrashHandler::Init() {
	signal(SIGINT, OnProcessCrashed);
	signal(SIGILL, OnProcessCrashed);
	signal(SIGSEGV, OnProcessCrashed);
	signal(SIGABRT, OnProcessCrashed);
	signal(SIGTERM, ShutdownEngine);
}

void CrashHandler::StackTrace(bool cutSetup, std::ostream& out) {
#ifdef PIT_WINDOWS
	HANDLE process = GetCurrentProcess();
	HANDLE thread = GetCurrentThread();

	CONTEXT context;
	memset(&context, 0, sizeof(CONTEXT));
	context.ContextFlags = CONTEXT_FULL;
	RtlCaptureContext(&context);

	SymInitialize(process, NULL, TRUE);

	DWORD image;
	STACKFRAME64 stackframe;
	ZeroMemory(&stackframe, sizeof(STACKFRAME64));

#ifdef _M_IX86
	image = IMAGE_FILE_MACHINE_I386;
	stackframe.AddrPC.Offset = context.Eip;
	stackframe.AddrPC.Mode = AddrModeFlat;
	stackframe.AddrFrame.Offset = context.Ebp;
	stackframe.AddrFrame.Mode = AddrModeFlat;
	stackframe.AddrStack.Offset = context.Esp;
	stackframe.AddrStack.Mode = AddrModeFlat;
#elif _M_X64
	image = IMAGE_FILE_MACHINE_AMD64;
	stackframe.AddrPC.Offset = context.Rip;
	stackframe.AddrPC.Mode = AddrModeFlat;
	stackframe.AddrFrame.Offset = context.Rsp;
	stackframe.AddrFrame.Mode = AddrModeFlat;
	stackframe.AddrStack.Offset = context.Rsp;
	stackframe.AddrStack.Mode = AddrModeFlat;
#elif _M_IA64
	image = IMAGE_FILE_MACHINE_IA64;
	stackframe.AddrPC.Offset = context.StIIP;
	stackframe.AddrPC.Mode = AddrModeFlat;
	stackframe.AddrFrame.Offset = context.IntSp;
	stackframe.AddrFrame.Mode = AddrModeFlat;
	stackframe.AddrBStore.Offset = context.RsBSP;
	stackframe.AddrBStore.Mode = AddrModeFlat;
	stackframe.AddrStack.Offset = context.IntSp;
	stackframe.AddrStack.Mode = AddrModeFlat;
#endif

	int i = 1;
	while (StackWalk64(image, process, thread, &stackframe, &context, NULL,	SymFunctionTableAccess64, SymGetModuleBase64, NULL)) {
		char buffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR)];
		PSYMBOL_INFO symbol = (PSYMBOL_INFO)buffer;
		symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
		symbol->MaxNameLen = MAX_SYM_NAME;

		char buffer2[sizeof(PIMAGEHLP_LINE64)];
		PIMAGEHLP_LINE64 lineCounter = (PIMAGEHLP_LINE64)buffer2; // contains data about a location in a file (filename, line, col, ...)
		lineCounter->SizeOfStruct = sizeof(PIMAGEHLP_LINE64);

		DWORD64 displacement = 0;
		[[maybe_unused]] DWORD displacement32 = 0;

		if (SymFromAddr(process, stackframe.AddrPC.Offset, &displacement, symbol)) {
			if (strcmp(symbol->Name, "abort") == 0)
				out << '[' << i << "] Internal function: abort()\n";
			else if (strcmp(symbol->Name, "raise") == 0)
				out << '[' << i << "] Internal function: raise()\n";
			else if (strcmp(symbol->Name, "CrashHandler::OnProcessCrashed") == 0 || strcmp(symbol->Name, "CrashHandler::StackTrace") == 0)
				continue;
			else {
				out << '[' << i << "] At " << symbol->Name << "()";
				#if RELEASE
				out << " in ";
				if (SymGetLineFromAddr64(process, stackframe.AddrPC.Offset, &displacement32, lineCounter))
					out << lineCounter->FileName << ":" << lineCounter->LineNumber;
				else
					out << " ???";
				#endif
				out << '\n';

				if (cutSetup && strcmp(symbol->Name, "main") == 0)
					break;
			}
		}
		else
			out << "[" << i << "] ???\n";
		i++;
	}
	SymCleanup(process);
#endif
}

void CrashHandler::OnProcessCrashed([[maybe_unused]] int sig) {
	std::cout << "A crash occured.\n";
	
	StackTrace(true);

	std::ofstream out(std::string("Logs/CrashReports/") + Pit::CurrentTimeToString() + "_Crash.log");
	if (out.is_open()) {
		out << "### System Info ###\n";
		out << "Platform: PC\n";
		out << "OS: " << Pit::OperatingSystem::ToString(Pit::OperatingSystem::Get()) << '\n';
		out << "Architecture: x64\n";
		out << "Cpu Processors: " << std::thread::hardware_concurrency() << '\n';
		out << "Ram: " << Pit::PhysicalStats::GetRam() / 1024 / 1024 << "GB\n";

		out << "\n### Crash Info ###\n";
		out << "Date: " << Pit::CurrentTimeToStringPretty() << '\n';
		const unsigned long currentProcId = Pit::Process::GetCurrentProcessID();
		out << "Process: " << Pit::StringFromWString(Pit::Process::GetName(currentProcId)) << "(" << currentProcId << ")\n";
		out << "Thread: " << (std::this_thread::get_id() == Pit::Thread::MainThreadId ? "Main Thread(" : "Additional Thread(") << std::this_thread::get_id() << ")\n";
		out << "Last recorded error msg: " << Pit::Debug::Logging::GetLastErrorMsg() << '\n';
		out << "Stacktrace:\n";
		StackTrace(true, out);
		out.close();
	}
	Pit::Engine::ForceShutdown();
}