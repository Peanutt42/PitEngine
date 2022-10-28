#pragma once

#include <cstdint>

typedef uint32_t uint32;
typedef uint64_t uint64;
typedef unsigned int uint;
typedef int32_t int32;
typedef int64_t int64;

template<typename To, typename From>
FORCEINLINE To Cast(From src) {
	return static_cast<To>(src);
}