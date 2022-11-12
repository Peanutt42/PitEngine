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
	
	inline static float Max(const float& a, const float& b) {
		return a > b ? a : b;
	}

	inline static float Min(const float& a, const float& b) {
		return a < b ? a : b;
	}

	inline static int Max(const int& a, const int& b) {
		return a > b ? a : b;
	}

	inline static int Min(const int& a, const int& b) {
		return a < b ? a : b;
	}

	inline static int64 Max(const int64& a, const int64& b) {
		return a > b ? a : b;
	}

	inline static int64 Min(const int64& a, const int64& b) {
		return a < b ? a : b;
	}

	inline static uint32 Max(const uint32& a, const uint32& b) {
		return a > b ? a : b;
	}

	inline static uint32 Min(const uint32& a, const uint32& b) {
		return a < b ? a : b;
	}

	inline static uint64 Max(const uint64& a, const uint64& b) {
		return a > b ? a : b;
	}

	inline static uint64 Min(const uint64& a, const uint64& b) {
		return a < b ? a : b;
	}
}