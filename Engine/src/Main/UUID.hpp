#pragma once

#include "Main/CoreInclude.hpp"
#include <xhash>

namespace Pit {
	typedef uint64_t uuid_type;

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
		std::size_t operator()(const Pit::UUID& uuid) const {
			return hash<Pit::uuid_type>()((Pit::uuid_type)uuid);
		}
	};
}