// Copied (and modified) from TheCherno/Hazel https://github.com/TheCherno/Hazel/blob/master/Hazel/src/Hazel/Core/UUID.h and https://github.com/TheCherno/Hazel/blob/master/Hazel/src/Hazel/Core/UUID.cpp

#pragma once

#include <xhash>
#include <random>

namespace Pit {
	using uuid_type = uint64_t;

	/// <summary>
	/// Random number for identifying stuff.
	/// Gets generated at construction
	/// </summary>
	class UUID {
	public:
		UUID() : m_UUID(s_UniformDistribution(s_Engine)) {}
		UUID(uuid_type uuid) : m_UUID(uuid) {}
		UUID(const UUID&) = default;

		operator uuid_type() const { return m_UUID; }

	private:
		uuid_type m_UUID;

		inline static std::random_device s_RandomDevice;
		inline static std::mt19937_64 s_Engine = std::mt19937_64(s_RandomDevice());
		inline static std::uniform_int_distribution<uuid_type> s_UniformDistribution;
	};
}

namespace std {
	template <typename T> struct hash;

	template<>
	struct hash<Pit::UUID> {
		std::size_t operator()(const Pit::UUID& uuid) const {
			return (uint64_t)uuid;
		}
	};

}