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

template<typename T>
class Ref {
public:
	Ref() : m_Value(nullptr) {}
	Ref(T* value) : m_Value { value } {}
	Ref(T value) : m_Value { &value } {}
	Ref(T& value) : m_Value { &value } {}

	T& operator*() { return *m_Value; }
	T* operator&() { return m_Value; }
	T* operator->() { return m_Value; }

	bool operator==(T* other) { return m_Value == other; }
	bool operator!=(T* other) { return m_Value != other; }

	explicit operator T() { return *m_Value; }
	explicit operator bool() { return m_Value != nullptr; }

private:
	unsigned short m_Uses;
	T* m_Value = nullptr;
};