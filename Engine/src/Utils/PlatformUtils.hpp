#pragma once

#include <string>
#ifdef PIT_WINDOWS
#include <windows.h>
#endif

namespace Pit {
	class MessagePrompts {
	public:
#ifdef PIT_WINDOWS
		static void InfoMessage(LPCWSTR msg);
		static void ErrorMessage(LPCWSTR msg);
#endif
	};
}