#pragma once

#include "Core/CoreInclude.hpp"

#if DEBUG
#define _CRTDBG_MAP_ALLOC
#define _CRTDBG_MAP_ALLOC_NEW
#define _INC_MALLOC
#include <crtdbg.h>
#include <assert.h>
#endif

namespace Pit::Debug::MemoryLeakDetector {
	
	static void Init() {
#if DEBUG
		_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
		//_CrtSetBreakAlloc(xyz); xyz -> {xyz} normal block at 0xWhatEverLocation, ... bytes long
#endif
	}

	static void PrintOutPotentialMemLeaks() {
#if DEBUG // Look at visual studio output after running, but don't be scared, most of it is PhysX
		_CrtDumpMemoryLeaks();
#endif
	}
}