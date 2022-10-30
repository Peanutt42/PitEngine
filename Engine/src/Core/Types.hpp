#pragma once

#include <cstdint>
#include <string>
#include <limits>

#define DECLARE_LIMITS(name, type) \
	constexpr type MAX_##name	= std::numeric_limits<type>::max();	\
	constexpr type MIN_##name	= std::numeric_limits<type>::min()

namespace Pit {
	using uint32 = uint32_t;
	using uint64 = uint64_t;
	using uint = unsigned int;
	DECLARE_LIMITS(UINT32, uint32);
	DECLARE_LIMITS(UINT64, uint64);
	DECLARE_LIMITS(UINT, uint);

	using int32 = int32_t;
	using int64 = int64_t;
	DECLARE_LIMITS(INT32, int32);
	DECLARE_LIMITS(INT64, int64);
	DECLARE_LIMITS(INT, int);

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