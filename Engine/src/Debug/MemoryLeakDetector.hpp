#pragma once

#include "Core/CoreInclude.hpp"

namespace Pit::Debug {
	class MemoryLeakDetector {
	public:
		static void Init();

		static void PrintOutPotentialMemLeaks();
	};
}