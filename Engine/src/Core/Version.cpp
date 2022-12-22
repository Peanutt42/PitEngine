#include "pch.hpp"
#include "Version.hpp"

namespace Pit {
	Version::Version(const unsigned int& major, const unsigned int& minor, const unsigned int& patch) : Major(major), Minor(minor), Patch(patch) {}
	
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