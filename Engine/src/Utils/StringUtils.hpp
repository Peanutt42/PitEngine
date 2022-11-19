#pragma once

#include <string>

namespace Pit {
	/// <returns>If s1 contains s2</returns>
	static FORCEINLINE bool StringContainsString(const String& s1, const String& s2) {
		return s1.find(s2) != String::npos;
	}
}