#pragma once

// Information from TheCherno/Hazel: PlatformDetection.h https://github.com/TheCherno/Hazel/blob/master/Hazel/src/Hazel/Core/PlatformDetection.h

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
		#error "MacOS is not supported yet, but it shouldn't be too difficult to change that"
		#ifndef PIT_MACOS	
			#define PIT_MACOS
		#endif
	#else
		#error "Unknown Apple platform!"
	#endif
#elif defined(__linux__)
	#error "Linux is not supported yet, but it shouldn't be too difficult to change that"
	#ifndef PIT_LINUX
		#define PIT_LINUX
	#endif
#else
	#error "Platform isn't supported!"
#endif