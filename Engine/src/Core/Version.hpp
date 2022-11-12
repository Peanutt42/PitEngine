#pragma once

#include "Core/CoreInclude.hpp"
#include "spdlog/fmt/ostr.h"

namespace Pit {
	struct Version {
		uint Major;
		uint Minor;
		uint Patch;
		
		explicit Version(uint major, uint minor, uint patch)
			: Major(minor), Minor(minor), Patch(patch) {}
	};
	static std::ostream& operator<<(std::ostream& os, const Version& version) {
		return os << version.Major << '.' << version.Minor << '.' << version.Patch;
	}
}