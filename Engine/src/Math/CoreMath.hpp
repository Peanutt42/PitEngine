#pragma once

// for simple float, int ... calculations

namespace Pit::Math {
	/// <summary>
	/// Makes sure that the value doesn't go under or over the defined limits
	/// </summary>
	/// <returns>Adjusted value based on the limits</returns>
	inline static float Clamp(const float& value, const float& min, const float& max) {
		if (value < min) return min;
		if (value > max) return max;
		return value;
	}

	/// <summary>
	/// Linearly interpolates between a and b by the weight t
	/// t is clamped between 0 and 1
	/// </summary>
	/// <returns>Linearly interpolated value</returns>
	inline static float Lerp(const float& a, const float& b, const float& t) {
		return a + Clamp(t, 0, 1) * (b - a);
	}
	
	/// <returns>Bigger Value</returns>
	inline static float Max(const float& a, const float& b) {
		return a > b ? a : b;
	}

	/// <returns>Smaller Value</returns>
	inline static float Min(const float& a, const float& b) {
		return a < b ? a : b;
	}

	/// <returns>Bigger Value</returns>
	inline static int Max(const int& a, const int& b) {
		return a > b ? a : b;
	}

	/// <returns>Smaller Value</returns>
	inline static int Min(const int& a, const int& b) {
		return a < b ? a : b;
	}

	/// <returns>Bigger Value</returns>
	inline static int64 Max(const int64& a, const int64& b) {
		return a > b ? a : b;
	}

	/// <returns>Smaller Value</returns>
	inline static int64 Min(const int64& a, const int64& b) {
		return a < b ? a : b;
	}

	/// <returns>Bigger Value</returns>
	inline static uint32 Max(const uint32& a, const uint32& b) {
		return a > b ? a : b;
	}

	/// <returns>Smaller Value</returns>
	inline static uint32 Min(const uint32& a, const uint32& b) {
		return a < b ? a : b;
	}

	/// <returns>Bigger Value</returns>
	inline static unsigned long long Max(const unsigned long long& a, const unsigned long long& b) {
		return a > b ? a : b;
	}

	/// <returns>Smaller Value</returns>
	inline static unsigned long long Min(const unsigned long long& a, const unsigned long long& b) {
		return a < b ? a : b;
	}
}