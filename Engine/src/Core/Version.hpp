#pragma once

#include <fmt/fmt.h>
#include <fmt/ostr.h>

namespace Pit {
	/// <summary>
	/// Version containing Major, Minor and Path number
	/// </summary>
	struct Version {
		unsigned int Major;
		unsigned int Minor;
		unsigned int Patch;
		
		Version(const unsigned int& major, const unsigned int& minor, const unsigned int& patch);
		Version(const String& string);

		void Print();

		const bool operator ==(const Version& other);
		const bool operator >(const Version& other);
		const bool operator <(const Version& other);
	};
#pragma warning(push)
#pragma warning(disable: 4505) // Not referenced function with internal references
	static std::ostream& operator<<(std::ostream& os, const Version& version) {
		return os << version.Major << '.' << version.Minor << '.' << version.Patch;
	}
#pragma warning(pop)
}