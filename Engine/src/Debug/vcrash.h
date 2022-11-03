/*
LICENSE:
MIT License

Copyright (c) 2020 vanyle

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

/**
	VCrash
	This is a windows only library used for debugging. (for MinGW64)
	It displays a stack trace on crashs.
	Include it once, call setup_crash_handler() and you're good to go!
	Also, if you want to print the stack at some point, just call:
	stack_trace(false,false);
	To make full use of this library, don't forget to compile with the -g option in include debugging symbols in your build.
	Moreover, generate a pdb file for your executable with cv2pdb for full debugging information.
	Without pdb file, I'm unable to retreive information like function names, or line numbers from the executable.
	Also, the line numbers are approximative and might not take header files / macros into account.
*/

#pragma once

#include <string.h>

#ifdef PIT_WINDOWS
#include <windows.h>
#include <DbgHelp.h>

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>


// on crash mode, don't print calls made after the crash occured
// on cut setup, don't print calls made before main is called.
static void stack_trace(bool crashMode, bool cutSetup) {
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

	bool printEnable = !crashMode;

	for (size_t i = 0; i < 25; i++) {
		BOOL result = StackWalk64(
			image, process, thread,
			&stackframe, &context, NULL,
			SymFunctionTableAccess64, SymGetModuleBase64, NULL);

		if (!result) { break; }

		char buffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR)];
		PSYMBOL_INFO symbol = (PSYMBOL_INFO)buffer;
		symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
		symbol->MaxNameLen = MAX_SYM_NAME;

		char buffer2[sizeof(PIMAGEHLP_LINE64)];
		PIMAGEHLP_LINE64 lineCounter = (PIMAGEHLP_LINE64)buffer2; // contains data about a location in a file (filename, line, col, ...)
		lineCounter->SizeOfStruct = sizeof(PIMAGEHLP_LINE64);

		DWORD64 displacement = 0;
		DWORD displacement32 = 0;

		if (SymFromAddr(process, stackframe.AddrPC.Offset, &displacement, symbol)) {

			if (printEnable) {
				fprintf(stderr, "At %s(...)", symbol->Name);

				// get line of symbol.
				if (SymGetLineFromAddr64(process, stackframe.AddrPC.Offset, &displacement32, lineCounter)) {
					fprintf(stderr, " (%s:%li)", lineCounter->FileName, lineCounter->LineNumber);
				}
				else {
					fprintf(stderr, " ???");
				}
				fprintf(stderr, "\n");
			}

			if (cutSetup && strcmp(symbol->Name, "main") == 0) {
				break;
			}
			if (!printEnable && (strcmp(symbol->Name, "abort") == 0 || strcmp(symbol->Name, "KiUserExceptionDispatcher") == 0)) {
				printEnable = true;
			}

		}
		else {
			fprintf(stderr, "[%lli] ???\n", i);
		}

	}
	SymCleanup(process);
}

void on_process_crash(int sig) {
	fprintf(stderr, "A crash occured.\n");
	fflush(stderr);

	stack_trace(true, true);
	fflush(stderr);
	exit(sig);
}
void setup_crash_handler() {
	signal(SIGSEGV, on_process_crash); // catch segfaults
	signal(SIGABRT, on_process_crash); // catch exceptions
}
#endif