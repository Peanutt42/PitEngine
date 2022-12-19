#pragma once

#include <cstdint>
#include <string>
#include <limits>

#include "SmartPointers.hpp"

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


	// Casts type with some compiler checks that the type is actually convertable
	template<typename To, typename From>
	__forceinline To Cast(From src) {
		static_assert((std::is_convertible_v<From, To> || std::is_convertible_v<To, From>) ||
					  (std::is_enum_v<To> || std::is_enum_v<From>) ||
					  (std::is_pointer_v<To> && std::is_pointer_v<From>), "Converting from 'From' to 'To' is not allowed!");
			
		return static_cast<To>(src);
	}
}