#pragma once

#include <iostream>

#if _WIN32 || _WIN64
#include <windows.h>
#else
#error "Platform to build to isn't supported!"
#endif

namespace Pit::Test {
#define PIT_TEST_MAIN() \
	int main() {												\
		HANDLE console_color;									\
		console_color = GetStdHandle(STD_OUTPUT_HANDLE);		\
		SetConsoleTextAttribute(console_color, 10);				\
		std::cout << "===PitTest===" << '\n';					\
		SetConsoleTextAttribute(console_color, 15);				\
	}
}