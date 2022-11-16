#pragma once

#include "Core/CoreInclude.hpp"
#include "spdlog/fmt/ostr.h"

namespace Pit {
	struct Version {
		uint Major;
		uint Minor;
		uint Patch;
		
		Version(const uint& major, const uint& minor, const uint& patch)
			: Major(major), Minor(minor), Patch(patch) {}

		void Print() {
			std::cout << this;
		}
	};
#pragma warning(push)
#pragma warning(disable: 4505) // Not referenced function with internal references
	static std::ostream& operator<<(std::ostream& os, const Version& version) {
		os << version.Major << '.' << version.Minor << '.' << version.Patch;
		return os;
	}
#pragma warning(pop)
}