#include "pch.hpp"
#include "Version.hpp"

namespace Pit {
	Version::Version(const unsigned int& major, const unsigned int& minor, const unsigned int& patch)
		: Major(major), Minor(minor), Patch(patch) {}

	Version::Version(const String& string) : Major(0), Minor(0), Patch(0) {
		String majorStr, minorStr, patchStr;
		bool hasMajor = false, hasMinor = false;
		for (int i = 0; i < string.size(); i++) {
			if (string[i] == '.') {
				if (!hasMajor) hasMajor = true;
				else hasMinor = true;
				continue;
			}
			if (!hasMajor) majorStr += string[i];
			else if (!hasMinor) minorStr += string[i];
			else patchStr += string[i];
		}
		Major = Cast<unsigned int>(atoi(majorStr.c_str()));
		Minor = Cast<unsigned int>(atoi(minorStr.c_str()));
		Patch = Cast<unsigned int>(atoi(patchStr.c_str()));
	}
	
	void Version::Print() {
		std::cout << this;
	}
	
	const bool Version::operator==(const Version& other) {
		return Major == other.Major && Minor == other.Minor && Patch == other.Patch;
	}
	
	const bool Version::operator>(const Version& other) {
		if (Major == other.Major) {
			if (Minor == other.Minor) {
				if (Patch == other.Patch)
					return false;
				return Patch > other.Patch;
			}
			return Minor > other.Minor;
		}
		return Major > other.Major;
	}
	
	const bool Version::operator<(const Version& other) {
		if (Major == other.Major) {
			if (Minor == other.Minor) {
				if (Patch == other.Patch)
					return false;
				return Patch < other.Patch;
			}
			return Minor < other.Minor;
		}
		return Major < other.Major;
	}
}