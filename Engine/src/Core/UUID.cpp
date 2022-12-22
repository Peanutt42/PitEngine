#include "pch.hpp"
#include "UUID.hpp"
#include <random>

namespace Pit {

	static std::random_device s_RandomDevice;
	static std::mt19937_64 s_Engine(s_RandomDevice());
	static std::uniform_int_distribution<uuid_type> s_UniformDistribution;

	UUID::UUID()
		: m_UUID(s_UniformDistribution(s_Engine)) {


	}

	UUID::UUID(uuid_type uuid)
		: m_UUID(uuid) {


	}
	
	UUID::operator uuid_type() const { return m_UUID; }
}