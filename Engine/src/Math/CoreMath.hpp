#pragma once

// for simple float, int ... calculations

namespace Pit::Math {
	inline static float Clamp(const float& value, const float& min, const float& max) {
		if (value < min) return min;
		if (value > max) return max;
		return value;
	}

	inline static float Lerp(const float& a, const float& b, const float& t) {
		return a + t * (b - a);
	}
}