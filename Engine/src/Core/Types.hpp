#pragma once

#include <cstdint>
#include <string>
#include <limits>

namespace Pit {
	using uint32 = uint32_t;
	using uint64 = uint64_t;
	using uint = unsigned int;

#define MAX_UINT32 UINT_MAX
#define MAX_UINT64 ULLONG_MAX
#define MAX_UINT UINT_MAX

	using int32 = int32_t;
	using int64 = int64_t;

#define MAX_INT32 INT_MAX
#define MIN_INT32 INT_MIN
#define MAX_INT64 LLONG_MAX
#define MIN_INT64 LLONG_MIN
#define MAX_INT INT_MAX
#define MIN_INT INT_MIN

	constexpr float EPSILON = std::numeric_limits<float>::epsilon();

	using size = size_t;

	using String = std::string;

	template<typename T>
	using Array = std::vector<T>;

	template<typename To, typename From>
	FORCEINLINE To Cast(From src) {
		return static_cast<To>(src);
	}
}