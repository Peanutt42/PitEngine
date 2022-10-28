#pragma once

// Inspired by TheCherno/Hazel: PlatformDetection.h https://github.com/TheCherno/Hazel/blob/master/Hazel/src/Hazel/Core/PlatformDetection.h

#if defined(__ANDROID__)
	#error "Android is not supported!"
#endif

#ifdef _WIN32
	#ifdef _WIN64
		#ifndef PIT_WINDOWS
			#define PIT_WINDOWS
		#endif
	#else
		#error "x86 Builds are not supported!"
	#endif

#elif defined(__APPLE__) || defined(__MACH__)
	#include <TargetConditionals.h>
	#if TARGET_IPHONE_SIMULATOR
		#error "IOS simulator is not supported!"
	#elif TARGET_OS_IPHONE
		#error "IOS is not supported!"
	#elif TARGET_OS_MAC
		#ifndef PIT_MACOS	
			#define PIT_MACOS
		#endif
	#else
		#error "Unknown Apple platform!"
	#endif
#elif defined(__linux__)
	#ifndef PIT_LINUX
		#define PIT_LINUX
	#endif
#else
	#error "Platform isn't supported!"
#endif