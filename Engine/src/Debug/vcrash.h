// From https://github.com/vanyle/vcrash/blob/master/src/vcrash.h with simple modifications to fit into PitEngine

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

class CrashHandler {
public:
	static void Init();
	// on crash mode, don't print calls made after the crash occured
	// on cut setup, don't print calls made before main is called.
	static void StackTrace(bool crashMode, bool cutSetup);

	static void OnProcessCrashed(int sig);
};