#pragma once

#include <string>

namespace Pit {
	/// <returns>If s1 contains s2</returns>
	static FORCEINLINE bool StringContainsString(const std::string& s1, const std::string& s2) {
		return s1.find(s2) != std::string::npos;
	}

	static FORCEINLINE std::string BytesToString(size_t bytes) {
		std::string output;
		if (bytes >= 1024 * 1024 * 1024) {
			output += std::to_string((float)bytes / (1024.f * 1024.f * 1024.f));
			output += " gb";
		}
		else if (bytes >= 1024 * 1024) {
			output += std::to_string((float)bytes / (1024.f * 1024.f));
			output += " mb";
		}
		else if (bytes >= 1024) {
			output += std::to_string((float)bytes / 1024.f);
			output += " kb";
		}
		else {
			output += std::to_string(bytes);
			output += " bytes";
		}
		return output;
	}
}