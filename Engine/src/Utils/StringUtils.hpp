#pragma once

#include <string>

namespace Pit {
	static FORCEINLINE bool StringContainsString(const String& s1, const String& s2) {
		return s1.find(s2) != String::npos;
	}
}