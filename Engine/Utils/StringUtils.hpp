#pragma once

#include <string>

namespace Pit {
	static FORCEINLINE bool StringContainsString(const std::string& s1, const std::string& s2) {
		return s1.find(s2) != std::string::npos;
	}
}