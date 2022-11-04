#pragma once

#include "PlatformDetection.hpp"

namespace Pit {
#if DEBUG || RELEASE
	#if defined(PIT_WINDOWS)
		#define PIT_DEBUGBREAK() __debugbreak()
	#elif defined(PIT_LINUX)
		#include <signal.h>
		#define PIT_DEBUGBREAK() raise(SIGTRAP)
	#else
		#error "Platform doesn't support debugbreak yet!"
	#endif
	#define PIT_ENABLE_ASSERTS
#else
	#define PIT_DEBUGBREAK()
#endif
}