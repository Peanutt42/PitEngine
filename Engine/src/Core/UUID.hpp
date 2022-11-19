#pragma once

#include "Core/CoreInclude.hpp"
#include <xhash>

namespace Pit {
	using uuid_type = uint64_t;

	/// <summary>
	/// Random number for identifing stuff.
	/// Gets generated at construction
	/// </summary>
	class UUID {
	public:
		UUID();
		UUID(uuid_type uuid);
		UUID(const UUID&) = default;

		operator uuid_type() const { return m_UUID; }

	private:
		uuid_type m_UUID;
	};
}

namespace std {
	template<>
	struct hash<Pit::UUID> {
		size_t operator()(const Pit::UUID& uuid) const {
			return hash<Pit::uuid_type>()((Pit::uuid_type)uuid);
		}
	};
}