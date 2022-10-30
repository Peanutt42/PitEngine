#pragma once

#include <string>
#include <chrono>
#include <format>

namespace Pit {
	inline String CurrentTimeToString() {
		const auto now = std::chrono::system_clock::now();
		return std::format("{:%d-%m-%Y__%H-%M-%OS}", now);
	}
}